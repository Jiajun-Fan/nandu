#pragma once
#include <string>
#include "hasher.hh"
#include "service.hh"

enum {
    SUB_AUTH_INIT,
    SUB_AUTH_HASH,
    SUB_AUTH_OK,
};

class AuthServerService : public Service, public Hasher {
public:
    int getServiceCode() const { return SVC_AUTH; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    explicit AuthServerService(const std::string& key);
    virtual ~AuthServerService() {}
private:
    std::string generateToken() const;
    static void handleAuthInit(Service* service, Session& session, const Operation& in);
    static void handleAuthHash(Service* service, Session& session, const Operation& in);

    EntryMap                            _entryMap;
};

class AuthClientService : public Service, public Hasher {
public:
    int getServiceCode() const { return SVC_AUTH; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    explicit AuthClientService(const std::string& key);
    virtual ~AuthClientService() {}

private:
    static void handleAuthInit(Service* service, Session& session, const Operation& in);
    static void handleAuthOK(Service* service, Session& session, const Operation& in);

    EntryMap                            _entryMap;
};
