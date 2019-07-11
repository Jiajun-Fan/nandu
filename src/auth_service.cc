#include "auth_service.hh"
#include <assert.h>
#include <sstream>
#include <iomanip>
#include "log.hh"
#include "exception.hh"

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
void AuthServerService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_AUTH_INIT:
        handleAuthInit(session, in, out);
        break;
    case OP_AUTH_HASH:
        handleAuthHash(session, in, out);
        break;
    default:
        assert(0);
    }
}

const OperationCode AuthServerService::_operations[] = {
    OP_AUTH_INIT,
    OP_AUTH_HASH,
    OP_BAD_OPERATION,
};

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

void AuthServerService::handleAuthInit(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_AUTH_INIT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }
    std::string token = generateToken();
    String2Package(token, out.getData());
    out.setOpCode(OP_AUTH_INIT);
    session.send = true;
    session.data = token;
    session.curState = S_AUTH_WAIT_HASH;
};

void AuthServerService::handleAuthHash(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_AUTH_WAIT_HASH) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }
    const std::string& token = session.data;
    std::string expectedHash = hash(token.c_str(), token.length());
    std::string hashStr;
    Package2String(in.getCdata(), hashStr);

    if (expectedHash == hashStr) {
        Info("S: Auth OK.\n");
        out.setOpCode(OP_AUTH_OK);
        session.curState = S_INIT;
    } else {
        Info("S: Auth BAD.\n");
        Debug("Expect hash %s.\n", expectedHash.c_str());
        Debug("Got hash %s.\n", hashStr.c_str());
        out.setOpCode(OP_DONE);
        String2Package("Bad hash string.", out.getData());
        session.curState = S_DONE;
    }
    session.send = true;
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
const OperationCode AuthClientService::_operations[] = {
    OP_AUTH_INIT,
    OP_AUTH_OK,
    OP_BAD_OPERATION,
};

void AuthClientService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_AUTH_INIT:
        handleAuthInit(session, in, out);
        break;
    case OP_AUTH_OK:
        handleAuthOK(session, in, out);
        break;
    default:
        assert(0);
    }
}

void AuthClientService::handleAuthInit(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_AUTH_INIT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }
    std::string token;
    std::string hashStr;
    Package2String(in.getCdata(), token);
    hashStr = hash(token.c_str(), token.length());
    String2Package(hashStr, out.getData());
    out.setOpCode(OP_AUTH_HASH);

    session.curState = C_AUTH_WAIT_RESULT;
    session.send = true;
}

void AuthClientService::handleAuthOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_AUTH_WAIT_RESULT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }
    session.curState = C_INIT;
    Info("C: Auth OK.\n");
}
