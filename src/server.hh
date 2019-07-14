#pragma once
#include <pthread.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <string>
#include <vector>
#include "service.hh"

#define kRemoteServer (false)
#define kLocalServer (true)

class Server;

struct Connection {
    struct sockaddr_in client_addr;
    Server* server;
    int fd;
    timer_t timerId;
};

typedef void (*SignalHandler)(int, siginfo_t*);

class Server : public ServiceManager {
public:
    Server(bool local, int port, int backlog);
    virtual ~Server();
    void run();
    void handleConnection(int fd);
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
};

Server* MakeServer(bool local, int port, int backlog);
Server* GetServer();
