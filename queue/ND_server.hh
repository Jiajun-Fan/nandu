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
    bool sendChallenge(int fd, const std::string& token);
    bool waitForHash(int fd, const std::string& token);
    bool waitForPushPop(int fd);
    std::string generateToken();
};
