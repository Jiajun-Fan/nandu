#pragma once
#include <string>
#include "hasher.hh"
#include "service.hh"

class AuthServerService : public Service, public Hasher {
public:
    ReasonCode handleAuthInit(Session& session, const Operation& in, Operation& out);
    ReasonCode handleAuthHash(Session& session, const Operation& in, Operation& out);

    explicit AuthServerService(const std::string& key) : 
        Hasher(key) {}
    virtual ~AuthServerService() {}
    virtual const OperationEntry* getOperationEntries() const { return _entryTable; }
private:
    std::string generateToken() const;
    static const OperationEntry _entryTable[];
};

class AuthClientService : public Service, public Hasher {
public:
    ReasonCode handleAuthInit(Session& session, const Operation& in, Operation& out);
    ReasonCode handleAuthOK(Session& session, const Operation& in, Operation& out);

    explicit AuthClientService(const std::string& key) : 
        Hasher(key) {}
    virtual ~AuthClientService() {}
    virtual const OperationEntry* getOperationEntries() const { return _entryTable; }
private:
    static const OperationEntry _entryTable[];
};
