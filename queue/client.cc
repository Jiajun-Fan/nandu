#include "client.hh"
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "log.hh"

Client::Client(const std::string& host, int port) :
                _port(port) {
    _addr = inet_addr(host.c_str());
    if (_addr < 0) {
        Fatal("Failed to get ip of \"%s\".\n", host.c_str());
        exit(-1);
    }
}

Client::~Client() {
}

void Client::push(Package& package) {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockFd > 0);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = _addr;
    servAddr.sin_port = htons(_port);

    if (::connect(sockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 0) {
        Error("Failed to create socket.\n");
        return;
    }
    
    push_(sockFd, package);

    close(sockFd);
}

void Client::pop(Package& package) {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockFd > 0);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = _addr;
    servAddr.sin_port = htons(_port);

    if (::connect(sockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 0) {
        Error("Failed to connect to server.\n");
        return;
    }
    
    pop_(sockFd, package);

    close(sockFd);
}
