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

void ServiceManager::runOperation(Session& session, const Operation& op, Operation& out) {

    int opCode = op.getOpCode();
    int svcCode = genSvcCode(opCode);
    auto sit = _servicesMap.find(svcCode);
    if (sit == _servicesMap.end()) {
        Debug("bad service %d\n", svcCode);
        throw Exception(RC_OP_UNSUPPORTED);
    }

    Service* service = sit->second;
    const Service::EntryMap& entryMap = service->getEntryMap();
    const Service::EntryMap::const_iterator eit = entryMap.find(opCode);
    if (eit == entryMap.end()) {
        Debug("service %d, bad operation %d\n",
                svcCode, opCode);
        throw Exception(RC_OP_UNSUPPORTED);
    }

    const Service::OperationEntry& entry = eit->second;
    if (session.curState != entry.expected) {
        Debug("service %d, operation %d, bad state %d, expect %d\n",
                svcCode, opCode,
                session.curState,
                entry.expected);
        throw Exception(RC_OP_WRONG_CODE);
    }

    entry.hd(service, session, op, out);
}
