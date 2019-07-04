#pragma once
#include <map>
#include <vector>
#include "reason_code.hh"
#include "session.hh"
#include "operation.hh"

typedef ReasonCode (*OperationHandler)(Session& session, const Operation& in, Operation& out);

typedef struct {
    OperationCode opCode;
    OperationHandler handler;
} OperationEntry;

class Service {
public:
    virtual const OperationEntry* getOperationEntries() const = 0;
    Service() {}
    virtual ~Service() {}
};

class ServiceManager {
public:
    ServiceManager() {}
    virtual ~ServiceManager();
    ReasonCode registerService(Service* service);
    ReasonCode runOperation(Session& session, const Operation& op);
    bool needAuth() const {
        return _opTable.find(OP_AUTH_INIT) != _opTable.end();
    }
    bool hasAuth() const {
        return _opTable.find(OP_AUTH_INIT) != _opTable.end();
    }
private:
    std::vector<Service*>                           _services;
    std::map<OperationCode, Service*>               _servicesMap;
    std::map<OperationCode, OperationHandler>       _opTable;
};
