#pragma once

#include "ND_io.hh"
#include "hasher.hh"
#include "client.hh"


class NanduClient : public Hasher, public Client {
public:
    NanduClient(const std::string& host, int port,
                 const std::string& key) :
                 Hasher(key), Client(host, port) {
    }
    virtual ~NanduClient() {};
    virtual void push_(int fd, Package& package);
    virtual void pop_(int fd, Package& package);

private:
    bool waitForChallenge(int fd, std::string& token);
    bool sendHash(int fd, const std::string& token);
};
