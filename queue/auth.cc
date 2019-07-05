#include "auth.hh"
#include <assert.h>
#include <sstream>
#include <iomanip>

// Auth Server
ReasonCode AuthServerService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_AUTH_INIT:
        return handleAuthInit(session, in, out);
    case OP_AUTH_HASH:
        return handleAuthHash(session, in, out);
    default:
        assert(0);
    }
    return RC_OK;
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

ReasonCode AuthServerService::handleAuthInit(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_AUTH_INIT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }
    ReasonCode code;
    std::string token = generateToken();
    CheckReasonCode(String2Operation(token, out));
    out.setOpCode(OP_AUTH_INIT);
    session.send = true;
    session.data = token;
    session.curState = S_AUTH_WAIT_HASH;

onExit:
    return code;
};

ReasonCode AuthServerService::handleAuthHash(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_AUTH_WAIT_HASH) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }
    ReasonCode code;
    const std::string& token = session.data;
    std::string expectedHash = hash(token.c_str(), token.length());
    std::string hashStr;
    CheckReasonCode(Operation2String(in, hashStr));

    if (expectedHash == hashStr) {
        Info("Auth OK.\n");
        out.setOpCode(OP_AUTH_OK);
        session.curState = S_DONE;
    } else {
        Info("Auth BAD.\n");
        Debug("Expect hash %s.\n", expectedHash.c_str());
        Debug("Got hash %s.\n", hashStr.c_str());
        out.setOpCode(OP_DONE);
        CheckReasonCode(String2Operation("Bad hash string.", out));
        session.curState = S_DONE;
    }
    session.send = true;

onExit:
    return code;
}

// Auth Client
const OperationCode AuthClientService::_operations[] = {
    OP_AUTH_INIT,
    OP_AUTH_OK,
    OP_BAD_OPERATION,
};

ReasonCode AuthClientService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_AUTH_INIT:
        return handleAuthInit(session, in, out);
    case OP_AUTH_OK:
        return handleAuthOK(session, in, out);
    default:
        assert(0);
    }
    return RC_OK;
}

ReasonCode AuthClientService::handleAuthInit(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_AUTH_INIT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }
    ReasonCode code;
    std::string token;
    std::string hashStr;
    CheckReasonCode(Operation2String(in, token));
    hashStr = hash(token.c_str(), token.length());
    CheckReasonCode(String2Operation(hashStr, out));
    out.setOpCode(OP_AUTH_HASH);

    session.curState = C_AUTH_WAIT_RESULT;
    session.send = true;
onExit:
    return code;
}

ReasonCode AuthClientService::handleAuthOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_AUTH_WAIT_RESULT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }
    session.curState = C_DONE;
    Info("Auth OK.\n");
    return RC_OK;
}
