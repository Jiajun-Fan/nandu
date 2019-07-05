#pragma once
#include <map>
#include <vector>
#include "reason_code.hh"
#include "session.hh"
#include "operation.hh"

class Service {
public:
    virtual const OperationCode* getOperations() const = 0;
    virtual ReasonCode handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out) = 0;
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
        return _servicesMap.find(OP_AUTH_INIT) != _servicesMap.end();
    }
    bool hasAuth() const {
        return _servicesMap.find(OP_AUTH_INIT) != _servicesMap.end();
    }
private:
    std::vector<Service*>                           _services;
    std::map<OperationCode, Service*>               _servicesMap;
};
