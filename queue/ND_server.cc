#include "ND_server.hh"
#include <limits.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <sstream>
#include "log.hh"
#include "ND_io.hh"
#include "time.h"


NanduServer::NanduServer(bool local, int port, 
            const std::string& key) :
            Hasher(key), Server(local, port) {
}

NanduServer::~NanduServer() {
}

std::string NanduServer::generateToken() {

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

ReasonCode NanduServer::sendChallenge(int fd, const std::string& token) {

    NanduOperation op = ND_CHALLENGE;
    ReasonCode code;
    CheckReasonCode(NanduReaderWriter(fd).write(op, token));

    Info("Token send %s.\n", token.c_str());
onExit:
    return code;
}

ReasonCode NanduServer::waitForHash(int fd, const std::string& token) {
    NanduOperation op;
    std::string hashStr;
    ReasonCode code; 
    std::string s;
    CheckReasonCode(NanduReaderWriter(fd).read(op, hashStr));
    if (op != ND_HASH) {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }

    s = hash(token.c_str(), token.length());
    if (s != hashStr) {
        op = ND_BAD_HASH;
        CheckReasonCode(NanduReaderWriter(fd).write(op, hashStr));
        CheckReasonCode(RC_ND_BAD_HASH);
    }

    op = ND_VALIDATED;
    CheckReasonCode(NanduReaderWriter(fd).write(op, hashStr));

    Info("Hash validated %s.\n", hashStr.c_str());
onExit:
    return code;
}

ReasonCode NanduServer::waitForPushPop(int fd) {
    NanduOperation op;
    ReasonCode code;
    Task task;
    Package taskPkg;
    CheckReasonCode(NanduReaderWriter(fd).read(op, taskPkg));

    if (op == ND_PUSH) {
        CheckReasonCode(CreateTaskFromPackage(taskPkg, task));
        pushTask((uint32_t)time(NULL), task);
        Info("Push task.\n", task.getName().c_str());
        task.printTask();
    } else if (op == ND_POP) {
        task = popTask();
        CheckReasonCode(task.package(taskPkg));
        CheckReasonCode(NanduReaderWriter(fd).write(ND_POP, taskPkg));
        Info("Pop task.\n", task.getName().c_str());
        task.printTask();
    } else {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }

onExit:
    return code;
}

void NanduServer::handleConnection(Connection* conn) {
    std::string token = generateToken();
    ReasonCode code;
    CheckReasonCode(sendChallenge(conn->fd, token));
    CheckReasonCode(waitForHash(conn->fd, token));
    CheckReasonCode(waitForPushPop(conn->fd));
onExit:
    printError(code);
    return;
}

