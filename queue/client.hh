#pragma once
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "io.hh"

class Client {
public:
    Client(const std::string& host, int port);
    virtual ~Client();
    void push(Package& package);
    void pop(Package& package);
    // to be overwritten in derived class
    virtual void push_(int fd, Package& package) = 0;
    virtual void pop_(int fd, Package& package) = 0;

private:
    in_addr_t                   _addr;
    int                         _port;
};
