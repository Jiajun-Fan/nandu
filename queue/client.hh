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

    ReasonCode start(Session& session);
    ReasonCode run(Session& session, const Operation& operation);
    ReasonCode end(Session& session);

private:
    in_addr_t                   _addr;
    int                         _port;
};
