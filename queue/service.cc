#include "service.hh"

ServiceManager::~ServiceManager() {
    for (auto it = _services.begin(); it != _services.end(); it++) {
        delete *it;
    }
};

ReasonCode ServiceManager::registerService(Service* service) {
    const OperationEntry* entry = service->getOperationEntries();
    while (entry->opCode != OP_BAD_OPERATION) {
        auto it = _opTable.find(entry->opCode);
        if (it != _opTable.end()) {
            return RC_SVC_DUP_OPCODE;
        }
        entry++;
    }
    entry = service->getOperationEntries();
    while (entry->opCode != OP_BAD_OPERATION) {
        _opTable[entry->opCode] = entry->handler;
        _servicesMap[entry->opCode] = service;
        entry++;
    }
    return RC_OK;
}

ReasonCode ServiceManager::runOperation(Session& session, const Operation& in) {
    Operation out;
    ReasonCode code;
    auto it = _opTable.find(in.opCode());
    if (it == _opTable.end()) {
        Debug("opcode %d\n", in.opCode());
        return RC_OP_UNSUPPORTED;
    }

    auto sit = _servicesMap.find(in.opCode());
    assert(sit != _servicesMap.end());
    session.service = sit->second;

    CheckReasonCode(it->second(session, in, out));
    if (session.send) {
        CheckReasonCode(out.write(session.fd));
    }
    // reset send, since send is not cleared by operation handler
    session.send = false;

onExit:
    return code;
}
