#include "service.hh"
#include "log.hh"
#include "exception.hh"

ServiceManager::~ServiceManager() {
    for (auto it = _servicesMap.begin(); it != _servicesMap.end(); it++) {
        delete it->second;
    }
};

void ServiceManager::registerService(Service* service) {
    int code = service->getServiceCode();
    auto it = _servicesMap.find(code);

    if (it != _servicesMap.end()) {
        throw Exception(RC_SVC_DUP_OPCODE);
    }
    _servicesMap[code] = service;
}

void ServiceManager::runOperation(Session& session, const Operation& op) {

    OperationCode opCode = op.opCode();
    auto sit = _servicesMap.find(opCode.getSvcCode());
    if (sit == _servicesMap.end()) {
        Debug("bad service %d\n", opCode.getSvcCode());
        throw Exception(RC_OP_UNSUPPORTED);
    }

    Service* service = sit->second;
    const Service::EntryMap& entryMap = service->getEntryMap();
    const Service::EntryMap::const_iterator eit = entryMap.find(opCode.getOpCode());
    if (eit == entryMap.end()) {
        Debug("service %d, bad operation %d\n",
                opCode.getSvcCode(), opCode.getOpCode());
        throw Exception(RC_OP_UNSUPPORTED);
    }

    const Service::OperationEntry& entry = eit->second;
    if (session.curState != entry.expected) {
        Debug("service %d, operation %d, bad state %d\n",
                opCode.getSvcCode(), opCode.getOpCode(),
                session.curState);
        throw Exception(RC_OP_WRONG_CODE);
    }

    entry.hd(service, session, op);
}
