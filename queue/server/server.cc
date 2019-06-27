#include "server.hh"
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include "queue.hh"
#include "sign.hh"
#include "error.hh"

struct Connection {
    struct sockaddr_in client_addr;
    Server* server;
    int fd;
};

static void* handleConnection(void* args) {
    Connection* conn = (Connection*) args;
    pthread_t tid = pthread_self();
    
    // do something

    conn->server->addThreadToBeJoin(tid);
    delete conn;
}

Server::Server(bool local, int port) : 
    _local(local), _port(port), _nb_conn(10) {
    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(_sock_fd > 0);
    pthread_mutex_init(&_lock, NULL);
}

Server::~Server() {
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
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(_port);
    int ret = ::bind(_sock_fd, (const struct sockaddr*)&servAddr, sizeof(servAddr));
    assert(ret == 0);
}

void Server::listen() {
    int ret = ::listen(_sock_fd, _nb_conn);
    assert(ret == 0);
}

void Server::run() {
    bind();
    listen();
    while (1) {
        joinThreads();
        Connection* conn = new Connection();
        socklen_t len = 0;
        conn->fd = accept(_sock_fd, (struct sockaddr*) &(conn->client_addr), &len);
        conn->server = this;
        pthread_t tid;
        pthread_create(&tid, NULL, handleConnection, conn);
    }
}

int main() {
    Task* t = new Task("test");
    IntParam* i1 = new IntParam(1);
    IntParam* i2 = new IntParam(2);
    IntParam* i3 = new IntParam(3);
    t->addParam(i1);
    t->addParam(i2);
    t->addParam(i3);
    Signer signer("fuck");
    std::string whatever("this is cool");
    std::cout << signer.sign(whatever.c_str(), whatever.length()) << std::endl;
    delete t;
    return 0;
}
