#pragma once
#include "server.hh"
#include "hasher.hh"
#include "queue.hh"


class NanduServer : public Hasher, public Server, public Queue {
public:
    NanduServer(bool local, int port, const std::string& key);
    virtual ~NanduServer();

    virtual void handleConnection(Connection* conn);

private:
    ReasonCode sendChallenge(int fd, const std::string& token);
    ReasonCode waitForHash(int fd, const std::string& token);
    ReasonCode waitForPushPop(int fd);
    std::string generateToken();
};
