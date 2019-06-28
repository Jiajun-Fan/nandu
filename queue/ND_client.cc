#include "ND_client.hh"
#include <string.h>
#include "log.hh"
#include "task.hh"

bool NanduClient::waitForChallenge(int fd, std::string& token) {
    NanduOperation op;
    PackageReasonCode code = NanduReaderWriter(fd).read(op, token);

    if (code != PKG_OK) {
        return false;
    }

    if (op != ND_CHALLENGE) {
        Error("Got wrong opcode %d.\n", op);
        return false;
    }

    Info("Got token %s.\n", token.c_str());

    return true;
}

bool NanduClient::sendHash(int fd, const std::string& token) {
    NanduOperation op = ND_HASH;
    std::string hashStr = hash(token.c_str(), token.length());

    PackageReasonCode code = NanduReaderWriter(fd).write(op, hashStr);
    if (code != PKG_OK) {
        return false;
    }
    Info("Hash send %s.\n", hashStr.c_str());
    return true;
}

void NanduClient::push_(int fd, Package& package) {
    std::string token;
    if (!waitForChallenge(fd, token) ||
            !sendHash(fd, token)) {
        Error("Failed to auth.\n");
        return;
    }

    std::string fuck("fuck");
    NanduOperation op = ND_PUSH;
    PackageReasonCode code = NanduReaderWriter(fd).write(op, fuck);
}

void NanduClient::pop_(int fd, Package& package) {
    std::string token;
    if (!waitForChallenge(fd, token) ||
            !sendHash(fd, token)) {
        Error("Failed to auth.\n");
        return;
    }

    NanduOperation op = ND_POP;
    PackageReasonCode code = NanduReaderWriter(fd).write(op, std::string(""));

    if (code == PKG_OK) {
        Package taskPkg;
        PackageReasonCode code = NanduReaderWriter(fd).read(op, taskPkg);
        if (code != PKG_OK) {
            return;
        }
        if (op != ND_PUSH) {
            Error("Got wrong opcode %d.\n", op);
            return;
        }
        Task task;
        TaskReasonCode tc = CreateTaskFromPackage(taskPkg, task);
        if (tc != TSK_OK) {
            return;
        }
        Info("Got task %s\n", task.getName().c_str());
    }
}
