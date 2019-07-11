#pragma once
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "service.hh"
#include "io.hh"
#include "session.hh"
#include "operation.hh"

class Client : public ServiceManager {
public:
    Client(const std::string& host, int port);
    virtual ~Client();

    Session start();
    void run(Session& session, const Operation& operation);
    void end(Session& session);

private:
    int connect();
    in_addr_t                   _addr;
    int                         _port;
};
