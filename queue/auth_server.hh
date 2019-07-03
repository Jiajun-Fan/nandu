#pragma once
#include "server.hh"
#include "hasher.hh"
#include "reason_code.hh"

class AuthServer : public Hasher, public Server { 
public:
    AuthServer(bool local, int port, const std::string& key);
    virtual ~AuthServer();

    virtual void handleConnection(Connection* conn);
    virtual ReasonCode handleOperation(Connection* conn) = 0;

private:
    ReasonCode auth(int fd);
    std::string generateToken();
    bool                                    _needAuth;
};
