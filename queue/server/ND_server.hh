#pragma once
#include "server.hh"
#include "sign.hh"
#include "queue.hh"

class NanduServer : public Signer, public Server, public Queue {
public:
    NanduServer(bool local, int port, 
            ConnectionHandler hd, const std::string& key);
    virtual ~NanduServer();
};
