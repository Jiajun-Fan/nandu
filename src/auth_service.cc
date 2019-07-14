#include "auth_service.hh"
#include <assert.h>
#include <sstream>
#include <iomanip>
#include "log.hh"
#include "exception.hh"

const int AuthEnum::kOpInit     = genOperationCode(SVC_AUTH, OP_AUTH_INIT);
const int AuthEnum::kOpHash     = genOperationCode(SVC_AUTH, OP_AUTH_HASH);
const int AuthEnum::kOpOK       = genOperationCode(SVC_AUTH, OP_AUTH_OK);
const int AuthEnum::kScWaitInit = genStateCode(SVC_AUTH, SC_AUTH_WAIT_INIT);
const int AuthEnum::kScWaitHash = genStateCode(SVC_AUTH, SC_AUTH_WAIT_HASH);
const int AuthEnum::kScWaitOK   = genStateCode(SVC_AUTH, SC_AUTH_WAIT_OK);

//  Auth Server State Machine
//
//                  hasAuth == true                         hash not expected
//  S_AUTH_INIT --+------------------> S_AUTH_WAIT_HASH --+-------------------> S_DONE
//                |                                       |
//                |                                       | hash expected
//                |                                       +----------------+
//                | hasAuth == false                                       |
//                +--------------------------------------------------------+--> S_INIT
//
AuthServerService::AuthServerService(const std::string& key) : 
                Hasher(key) {
    _entryMap[kOpInit] = OperationEntry(handleAuthInit, kScWaitInit);
    _entryMap[kOpHash] = OperationEntry(handleAuthHash, kScWaitHash);
}

std::string AuthServerService::generateToken() const {

    static bool inited = false;
    if (!inited) {
        srandom((unsigned)time(NULL)*356821);
        inited = true;
    }
    
    std::stringstream ss;
    for (unsigned j = 0; j < 4; j++) {
        long r = random();
        unsigned char* p = (unsigned char*)&r;
        for (unsigned i = 0; i < sizeof(long); i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)(p[i]);
        }
    }
    return ss.str();
}

void AuthServerService::handleAuthInit(Service* service, Session& session, const Operation& in) {
    AuthServerService* svc = dynamic_cast<AuthServerService*>(service);
    assert(svc);

    std::string token = svc->generateToken();
    Operation out(kOpInit);
    out.fromString(token);
    session.data = token;
    session.curState = kScWaitHash;
    out.write(session.fd);
};

void AuthServerService::handleAuthHash(Service* service, Session& session, const Operation& in) {
    AuthServerService* svc = dynamic_cast<AuthServerService*>(service);
    assert(svc);

    const std::string& token = session.data;
    std::string expectedHash = svc->hash(token.c_str(), token.length());
    std::string hashStr = in.toString();

    if (expectedHash == hashStr) {
        Info("S: Auth OK.\n");
        Operation(kOpOK).write(session.fd);
        session.curState = SC_INIT;
    } else {
        Info("S: Auth BAD.\n");
        Debug("Expect hash %s.\n", expectedHash.c_str());
        Debug("Got hash %s.\n", hashStr.c_str());

        Operation out(OP_DONE);
        out.fromString(std::string("Bad hash string"));
        out.write(session.fd);

        session.curState = SC_DONE;
    }
    session.data = "";
}

//  Auth Client State Machine
//
//                  OP_AUTH_INIT (has key)                          OP_DONE
//  C_AUTH_INIT --+------------------------> C_AUTH_WAIT_RESULT --+--------------> C_DONE
//                |                                               |
//                |                                               | OP_AUTH_OK
//                |                                               +------------+
//                | OP_AUTH_INIT (no key)                                      |
//                +------------------------------------------------------------+-> C_INIT
//
AuthClientService::AuthClientService(const std::string& key) : 
                Hasher(key) {
    _entryMap[kOpInit] = OperationEntry(handleAuthInit, kScWaitInit);
    _entryMap[kOpOK] = OperationEntry(handleAuthOK, kScWaitOK);
}

void AuthClientService::handleAuthInit(Service* service, Session& session, const Operation& in) {
    std::string token = in.toString();
    std::string hashStr;
    AuthClientService* svc = dynamic_cast<AuthClientService*>(service);
    assert(svc);
    hashStr = svc->hash(token.c_str(), token.length());
    Operation out(kOpHash);
    out.fromString(hashStr);
    out.write(session.fd);

    session.curState = kScWaitOK;
}

void AuthClientService::handleAuthOK(Service* service, Session& session, const Operation& in) {
    session.curState = SC_INIT;
    Info("C: Auth OK.\n");
}
