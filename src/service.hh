#pragma once
#include <map>
#include <vector>
#include "session.hh"
#include "operation.hh"
#include "session.hh"

class Service {
public:
    typedef void (*OperationHandler)(Service* service, Session& session, const Operation& in, Operation& out);
    class OperationEntry {
    public:
        OperationEntry() : hd(NULL), expected(0) {}
        OperationEntry(OperationHandler hd_, int expected_) :
                        hd(hd_), expected(expected_) {
        }
        OperationHandler hd;
        int expected;
    };

    typedef std::map<int, OperationEntry> EntryMap;

    virtual int getServiceCode() const = 0;
    virtual const EntryMap& getEntryMap() const = 0;

    Service() {}
    virtual ~Service() {}
};

class ServiceManager {
public:
    ServiceManager() {}
    virtual ~ServiceManager();
    void registerService(Service* service);
    void runOperation(Session& session, const Operation& op, Operation& out);
    bool needAuth() const {
        return _servicesMap.find(SVC_AUTH) != _servicesMap.end();
    }
    bool hasAuth() const {
        return _servicesMap.find(SVC_AUTH) != _servicesMap.end();
    }
private:
    std::map<int, Service*>                       _servicesMap;
};
