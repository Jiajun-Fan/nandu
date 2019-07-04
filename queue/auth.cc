#include "auth.hh"
#include <sstream>
#include <iomanip>

// Auth Server
static ReasonCode AuthServerInitHandler(Session& session, const Operation& in, Operation& out) {
    AuthServerService* service = dynamic_cast<AuthServerService*>(session.service);
    assert(service);
    return service->handleAuthInit(session, in, out);
}

static ReasonCode AuthServerHashHandler(Session& session, const Operation& in, Operation& out) {
    AuthServerService* service = dynamic_cast<AuthServerService*>(session.service);
    assert(service);
    return service->handleAuthHash(session, in, out);
}

const OperationEntry AuthServerService::_entryTable[] = {
    { OP_AUTH_INIT, AuthServerInitHandler },
    { OP_AUTH_HASH, AuthServerHashHandler },
    { OP_BAD_OPERATION, NULL }
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
static ReasonCode AuthClientInitHandler(Session& session, const Operation& in, Operation& out) {
    AuthClientService* service = dynamic_cast<AuthClientService*>(session.service);
    assert(service);
    return service->handleAuthInit(session, in, out);
}

static ReasonCode AuthClientOKHandler(Session& session, const Operation& in, Operation& out) {
    AuthClientService* service = dynamic_cast<AuthClientService*>(session.service);
    assert(service);
    return service->handleAuthOK(session, in, out);
}

const OperationEntry AuthClientService::_entryTable[] = {
    { OP_AUTH_INIT, AuthClientInitHandler },
    { OP_AUTH_OK, AuthClientOKHandler },
    { OP_BAD_OPERATION, NULL }
};

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
