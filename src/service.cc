#include "service.hh"

ServiceManager::~ServiceManager() {
    for (auto it = _services.begin(); it != _services.end(); it++) {
        delete *it;
    }
};

ReasonCode ServiceManager::registerService(Service* service) {
    const OperationCode* code = service->getOperations();
    while (*code != OP_BAD_OPERATION) {
        auto it = _servicesMap.find(*code);
        if (it != _servicesMap.end()) {
            return RC_SVC_DUP_OPCODE;
        }
        _servicesMap[*code] = service;
        code++;
    }
    return RC_OK;
}

ReasonCode ServiceManager::runOperation(Session& session, const Operation& in) {
    Operation out;
    ReasonCode code;

    auto sit = _servicesMap.find(in.opCode());
    if (sit == _servicesMap.end()) {
        Debug("opcode %d\n", in.opCode());
        return RC_OP_UNSUPPORTED;
    }
    assert(sit != _servicesMap.end());
    CheckReasonCode(sit->second->handleOperation(in.opCode(), session, in, out));

    if (session.send) {
        CheckReasonCode(out.write(session.fd));
    }
    // reset send, since send is not cleared by operation handler
    session.send = false;

onExit:
    return code;
}
