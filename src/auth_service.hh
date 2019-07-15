#pragma once
#include <string>
#include "hasher.hh"
#include "service.hh"

class AuthEnum {
private:
enum {
    SC_AUTH_WAIT_INIT,
    SC_AUTH_WAIT_HASH,
    SC_AUTH_WAIT_OK,
};

enum {
    OP_AUTH_INIT,
    OP_AUTH_HASH,
    OP_AUTH_OK,
};

public:
    static const int kOpInit;
    static const int kOpHash;
    static const int kOpOK;
    static const int kScWaitInit;
    static const int kScWaitHash;
    static const int kScWaitOK;
};

class AuthServerService : public Service, public Hasher, public AuthEnum {
public:
    int getServiceCode() const { return SVC_AUTH; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    explicit AuthServerService(const std::string& key);
    virtual ~AuthServerService() {}
    static int kOpAuthInit;
    static int kOpAuthHash;
    static int kScAuthWaitInit;
    static int kScAuthWaitHash;
private:
    std::string generateToken() const;
    static void handleAuthInit(Service* service, Session& session, const Operation& in, Operation& out);
    static void handleAuthHash(Service* service, Session& session, const Operation& in, Operation& out);

    EntryMap                            _entryMap;
};

class AuthClientService : public Service, public Hasher, public AuthEnum {
public:
    int getServiceCode() const { return SVC_AUTH; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    explicit AuthClientService(const std::string& key);
    virtual ~AuthClientService() {}

    static int kOpAuthInit;
    static int kOpAuthOK;
    static int kScAuthWaitInit;
    static int kScAuthWaitOK;
private:
    static void handleAuthInit(Service* service, Session& session, const Operation& in, Operation& out);
    static void handleAuthOK(Service* service, Session& session, const Operation& in, Operation& out);

    EntryMap                            _entryMap;
};
