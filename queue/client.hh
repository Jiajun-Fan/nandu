#pragma once
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>

class Client {
public:
    Client(const std::string& host, int port);
    virtual ~Client();
    void connect();
    virtual void handleConnection(int fd) = 0;

private:
    in_addr_t                   _addr;
    int                         _port;
};
