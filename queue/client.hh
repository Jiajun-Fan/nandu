#pragma once
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "service.hh"
#include "io.hh"
#include "session.hh"

class Client : public ServiceManager {
public:
    Client(const std::string& host, int port);
    virtual ~Client();

    void run();
    //ReasonCode runOperation(const Operation& out);

private:
    in_addr_t                   _addr;
    int                         _port;
};
