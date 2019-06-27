#pragma once
#include "server.hh"
#include "sign.hh"
#include "queue.hh"

class NanduServer : public Signer, public Server, public Queue {
public:
    NanduServer(bool local, int port, 
            ConnectionFunctor functor, const std::string& key);
    virtual ~NanduServer();
};
