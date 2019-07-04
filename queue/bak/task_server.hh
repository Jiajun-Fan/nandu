#pragma once
#include "auth_server.hh"
#include "queue.hh"

class TaskServer :  public AuthServer, public Queue {
public:
    TaskServer(bool local, int port, const std::string& key) :
        AuthServer(local, port, key) {}
    virtual ~TaskServer() {}
    virtual ReasonCode handleConnection(int fd);
};
