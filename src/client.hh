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

    void start(Session& session);
    void run(Session& session, const Operation& operation, Operation& out);
    void end(Session& session);

private:
    int connect();
    in_addr_t                   _addr;
    int                         _port;
};
