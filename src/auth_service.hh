#pragma once
#include <string>
#include "hasher.hh"
#include "service.hh"

class AuthServerService : public Service, public Hasher {
public:
    virtual ReasonCode handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    explicit AuthServerService(const std::string& key) : 
        Hasher(key) {}
    virtual ~AuthServerService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    std::string generateToken() const;
    ReasonCode handleAuthInit(Session& session, const Operation& in, Operation& out);
    ReasonCode handleAuthHash(Session& session, const Operation& in, Operation& out);

    static const OperationCode _operations[];
};

class AuthClientService : public Service, public Hasher {
public:
    virtual ReasonCode handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    explicit AuthClientService(const std::string& key) : 
        Hasher(key) {}
    virtual ~AuthClientService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    ReasonCode handleAuthInit(Session& session, const Operation& in, Operation& out);
    ReasonCode handleAuthOK(Session& session, const Operation& in, Operation& out);

    static const OperationCode _operations[];
};
