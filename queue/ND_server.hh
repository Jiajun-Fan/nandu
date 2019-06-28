#pragma once
#include "server.hh"
#include "sign.hh"
#include "queue.hh"
#include "package.hh"

typedef enum {
    OpChallenge,
    OpHash,
    OpPush,
    OpPop
} NanduOperation;

class NanduServer : public Signer, public Server, public Queue {
public:
    bool sendChallenge(int fd, const std::string& token);
    bool waitForHash(int fd, const std::string& token);
    bool waitForPushPop();
    std::string generateToken();
    virtual void handleConnection(Connection* conn);

    NanduServer(bool local, int port, const std::string& key);
    virtual ~NanduServer();
};
