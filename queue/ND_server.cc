#include "ND_server.hh"
#include <limits.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <sstream>
#include "log.hh"
#include "ND_io.hh"


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
    long r = random();
    unsigned char* p = (unsigned char*)&r;
    for (unsigned i = 0; i < sizeof(long); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)(p[i]);
    }
    return ss.str();
}

bool NanduServer::sendChallenge(int fd, const std::string& token) {

    NanduOperation op = ND_CHALLENGE;
    PackageReasonCode code = NanduReaderWriter(fd).write(op, token);

    if (code != PKG_OK) {
        return false;
    }

    Info("Token send %s.\n", token.c_str());
    return true;
}

bool NanduServer::waitForHash(int fd, const std::string& token) {
    NanduOperation op;
    std::string hashStr;
    PackageReasonCode code = NanduReaderWriter(fd).read(op, hashStr);
    if (code != PKG_OK) {
        return false;
    }

    if (op != ND_HASH) {
        Error("Got wrong opcode %d.\n", op);
        return false;
    }

    std::string s = hash(token.c_str(), token.length());
    if (s != hashStr) {
        Error("Bad hash %s.\n", hashStr.c_str());
        return false;
    }
    Info("Hash validated %s.\n", hashStr.c_str());
    return true;
}

bool NanduServer::waitForPushPop(int fd) {
    NanduOperation op;
    std::string msg;
    PackageReasonCode code = NanduReaderWriter(fd).read(op, msg);
    if (code != PKG_OK) {
        return false;
    }
    if (op == ND_PUSH) {
        Info("Got push %s.\n", msg.c_str());
    } else if (op == ND_POP) {
        std::unique_ptr<Task> task = popTask();
        Package taskPkg;
        TaskReasonCode tc = task->package(taskPkg);
        if (tc == TSK_OK) {
            code = NanduReaderWriter(fd).write(ND_PUSH, taskPkg);
            if (code == PKG_OK) {
                Info("Pop task %s.\n", task->getName().c_str());
            }
        }
    } else {
        Error("Got wrong opcode %d.\n", op);
        return false;
    }
    return true;
}

void NanduServer::handleConnection(Connection* conn) {
    std::string token = generateToken();
    if (!sendChallenge(conn->fd, token) ||
            !waitForHash(conn->fd, token) ||
            !waitForPushPop(conn->fd)) {
        Error("Bad request.\n");
        return;
    }
}

