#include "auth_server.hh"
#include <limits.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <sstream>
#include "log.hh"
#include "operation_io.hh"
#include "time.h"


AuthServer::AuthServer(bool local, int port, 
            const std::string& key) :
            Hasher(key), Server(local, port),
            _needAuth(key!="") {
}

AuthServer::~AuthServer() {
}

std::string AuthServer::generateToken() {

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

ReasonCode AuthServer::auth(int fd) {

    std::string token = generateToken();
    Operation op = ND_CHALLENGE;
    ReasonCode code;

    std::string hashStr;
    std::string s;

    // send token
    CheckReasonCode(OperationReaderWriter(fd).write(op, token));
    Info("Token send %s.\n", token.c_str());

    // recieve package
    CheckReasonCode(OperationReaderWriter(fd).read(op, hashStr));
    if (op != ND_HASH) {
        Debug("Got opcode %d.\n", op);
        CheckReasonCode(RC_ND_WRONG_CODE);
    }

    // validate hash
    s = hash(token.c_str(), token.length());
    if (s != hashStr) {
        op = ND_BAD_HASH;
        CheckReasonCode(OperationReaderWriter(fd).write(op, hashStr));
        CheckReasonCode(RC_ND_BAD_HASH);
    } else {
        op = ND_VALIDATED;
        CheckReasonCode(OperationReaderWriter(fd).write(op, hashStr));
        Info("Hash validated %s.\n", hashStr.c_str());
    }
onExit:
    return code;
}

void AuthServer::handleConnection(Connection* conn) {
    ReasonCode code;
    if (_needAuth) {
        CheckReasonCode(auth(conn->fd));
    }
    CheckReasonCode(handleOperation(conn));
onExit:
    printError(code);
    return;
}

