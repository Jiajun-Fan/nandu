#pragma once
#include <pthread.h>
#include <string>
#include <vector>

class Server {
public:
    Server(bool local, int port);
    virtual ~Server();
    void run();
    void addThreadToBeJoin(const pthread_t& tid);

private:
    void bind();
    void listen();
    void joinThreads();

    bool                        _local;
    int                         _port;
    int                         _sock_fd;
    int                         _nb_conn;
    pthread_mutex_t             _lock;
    std::vector<pthread_t>      _tids;
};
