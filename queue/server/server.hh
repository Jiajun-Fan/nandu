#pragma once
#include <pthread.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <string>
#include <vector>

class Server;

struct Connection {
    struct sockaddr_in client_addr;
    Server* server;
    int fd;
    timer_t timerId;
};

typedef void* (*ConnectionFunctor)(Connection*);
typedef void (*SignalFunctor)(int, siginfo_t*);

class Server {
public:
    Server(bool local, int port, ConnectionFunctor functor);
    virtual ~Server();
    void run();
    void runConnectionFunctor(Connection* conn);
    void addThreadToBeJoin(const pthread_t& tid);

private:
    void bind();
    void listen();
    void joinThreads();
    void setupSignalHandler();

    bool                        _local;
    int                         _port;
    int                         _sock_fd;
    int                         _nb_conn;
    pthread_mutex_t             _lock;
    std::vector<pthread_t>      _tids;
    ConnectionFunctor           _functor;
};
