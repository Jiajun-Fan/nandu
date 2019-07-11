#include "service.hh"
#include "log.hh"
#include "exception.hh"

ServiceManager::~ServiceManager() {
    for (auto it = _services.begin(); it != _services.end(); it++) {
        delete *it;
    }
};

void ServiceManager::registerService(Service* service) {
    const OperationCode* code = service->getOperations();
    while (*code != OP_BAD_OPERATION) {
        auto it = _servicesMap.find(*code);
        if (it != _servicesMap.end()) {
            throw Exception(RC_SVC_DUP_OPCODE);
        }
        _servicesMap[*code] = service;
        code++;
    }
}

void ServiceManager::runOperation(Session& session, const Operation& in) {
    Operation out;

    auto sit = _servicesMap.find(in.opCode());
    if (sit == _servicesMap.end()) {
        Debug("opcode %d\n", in.opCode());
        throw Exception(RC_OP_UNSUPPORTED);
    }
    assert(sit != _servicesMap.end());
    sit->second->handleOperation(in.opCode(), session, in, out);

    if (session.send) {
        out.write(session.fd);
    }
    // reset send, since send is not cleared by operation handler
    session.send = false;
}
