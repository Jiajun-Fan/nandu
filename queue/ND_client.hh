#pragma once

#include "ND_server.hh"
#include "sign.hh"
#include "client.hh"


class NanduClient : public Signer, public Client {
public:
    bool waitForChallenge(int fd, std::string& token);
    bool sendHash(int fd, const std::string& token);

    virtual void handleConnection(int fd);
    NanduClient(const std::string& host, int port,
                 const std::string& key) :
                 Signer(key), Client(host, port) {
    }
    virtual ~NanduClient() {};
};

