#include "server.hh"
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <atomic>
#include <iostream>
#include "auth_service.hh"
#include "exception.hh"
#include "log.hh"

const int kTimeout = 10;
static Server* kSingletonServer = NULL;

Server* MakeServer(bool local, int port, int backlog) {
    if (kSingletonServer != NULL) {
        Fatal("Only one server is allowed!\n");
        exit(-1);
    }
    kSingletonServer = new Server(local, port, backlog);
    return kSingletonServer;
}

Server* GetServer() {
    return kSingletonServer;
}

static void cleanUpThread(Connection* conn) {
    static std::atomic<bool> called(false);
    if (called.load()) {
        return;
    }
    called.store(true);

    timer_delete(conn->timerId);
    pthread_t tid = pthread_self();
    conn->server->addThreadToBeJoin(tid);
    int ret = close(conn->fd);
    delete conn;
    called.store(false);
    pthread_exit(NULL);
}

static void setupThread(Connection* conn) {
    // handle case of timeout
    struct sigevent sigEvent;
    memset(&sigEvent, 0, sizeof(sigEvent));
    sigEvent.sigev_notify = SIGEV_THREAD_ID;
    sigEvent.sigev_signo = SIGALRM;
    sigEvent.sigev_value.sival_ptr = (void*)conn;
    sigEvent._sigev_un._tid = syscall(SYS_gettid);

    timer_create(CLOCK_REALTIME, &sigEvent, &(conn->timerId));
    
    struct itimerspec timerSpec;
    memset(&timerSpec, 0, sizeof(timerSpec));
    timerSpec.it_value.tv_sec = kTimeout;

#ifdef NDEBUG
    timer_settime(conn->timerId, 0, &timerSpec, NULL);
#endif
}

static void* handleConnection(void* args) {
    Connection* conn = (Connection*) args;

    // create timer ...
    setupThread(conn);

    // run the handler
    try {
        conn->server->handleConnection(conn->fd);
    } catch (Exception e) {
        Error("%s\n", e.what());
    }

    // free resource
    cleanUpThread(conn);

    return 0;
}

static void TimeoutHandler(int sig, siginfo_t* info, void* uc) {
    Connection* conn = (Connection*)info->si_value.sival_ptr;
    cleanUpThread(conn);
}

static void IntruptedHandler(int sig) {
    if (kSingletonServer != NULL) {
        delete kSingletonServer;
        kSingletonServer = NULL;
    }
    exit(-1);
}

Server::Server(bool local, int port, int backlog) : 
    _local(local), _port(port), 
    _nb_conn(backlog) {
    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(_sock_fd > 0);
    pthread_mutex_init(&_lock, NULL);
}

Server::~Server() {
    if (_sock_fd) {
        int ret = close(_sock_fd);
        Debug("Socket %d closed\n", _sock_fd);
        _sock_fd = 0;
    }
    pthread_mutex_destroy(&_lock);
}

void Server::joinThreads() {
    pthread_mutex_lock(&_lock);
    for (auto it = _tids.begin(); it != _tids.end(); it++) {
        pthread_join(*it, NULL);
    }
    _tids.clear();
    pthread_mutex_unlock(&_lock);
}

void Server::addThreadToBeJoin(const pthread_t& tid) {
    pthread_mutex_lock(&_lock);
    _tids.push_back(tid);
    pthread_mutex_unlock(&_lock);
}

void Server::bind() {
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    if (_local) {
        servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    } else {
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    servAddr.sin_port = htons(_port);
   
    int ret = ::bind(_sock_fd, (const struct sockaddr*)&servAddr, sizeof(servAddr));
    if (ret != 0) {
        if (errno == EACCES) {
            Fatal("The port :%d is protected.\n", _port);
        } else if (errno == EADDRINUSE) { 
            Fatal("The port :%d is already used.\n", _port);
        } else {
            Fatal("Failed to bind to address :%d.\n", _port);
        }
        exit(-1);
    }
    assert(ret == 0);
}

void Server::listen() {
    int ret = ::listen(_sock_fd, _nb_conn);
    assert(ret == 0);
}

void Server::setupSignalHandler() {
    // setup SIGALRM
    struct sigaction saAlarm;
    memset(&saAlarm, 0, sizeof(saAlarm));
    saAlarm.sa_sigaction = TimeoutHandler;
    saAlarm.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &saAlarm, NULL);

    struct sigaction saInt;
    memset(&saInt, 0, sizeof(saInt));

    saInt.sa_handler = IntruptedHandler;
    sigaction(SIGINT, &saInt, NULL);
}

void Server::run() {
    bind();
    listen();
    setupSignalHandler();

    while (1) {
        joinThreads();
        Connection* conn = new Connection();
        socklen_t len = 0;
        conn->fd = accept(_sock_fd, (struct sockaddr*) &(conn->client_addr), &len);
        conn->server = this;
        pthread_t tid;
        pthread_create(&tid, NULL, ::handleConnection, conn);
    }
}

void Server::handleConnection(int fd) {

    Session session = { fd, SC_INIT, "" };
    // all the operations performed on server side don't
    // have output
    Operation dummy;

    if (needAuth()) {
        session.curState = AuthEnum::kScWaitInit;
        runOperation(session, Operation(AuthEnum::kOpInit), dummy);
    } else {
        Operation(AuthEnum::kOpInit).write(session.fd);
    }

    while (session.curState != SC_DONE) {
        Operation in;
        in.read(session.fd);
        if (in.getOpCode() == OP_DONE) {
            //Debug("%s\n", in.toString().c_str());
            break;
        }
        runOperation(session, in, dummy);
    }
}
