#pragma once

#include "operation_io.hh"
#include "hasher.hh"
#include "client.hh"


class TaskClient : public Hasher, public Client {
public:
    TaskClient(const std::string& host, int port,
                 const std::string& key) :
                 Hasher(key), Client(host, port) {
    }
    virtual ~TaskClient() {};
    virtual void push_(int fd, Package* package);
    virtual void pop_(int fd, Package* package);

private:
    ReasonCode waitForChallenge(int fd, std::string& token);
    ReasonCode sendHash(int fd, const std::string& token);
};
