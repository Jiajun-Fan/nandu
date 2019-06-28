#include "client.hh"
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "error.hh"

Client::Client(const std::string& host, int port) :
                _port(port) {
    _addr = inet_addr(host.c_str());
    if (_addr < 0) {
        Error() << "failed to get ip of \"" << host << "\"." << std::endl;
        exit(-1);
    }
}

Client::~Client() {
}

void Client::connect() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockFd > 0);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = _addr;
    servAddr.sin_port = htons(_port);

    if (::connect(sockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 0) {
        Error() << "Failed to connect to server." << std::endl;
        exit(-1);
    }
    handleConnection(sockFd);

    close(sockFd);
}
