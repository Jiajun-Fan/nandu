#include "ND_client.hh"
#include <string.h>
#include "log.hh"
#include "task.hh"

ReasonCode NanduClient::waitForChallenge(int fd, std::string& token) {
    NanduOperation op;
    ReasonCode code;
    CheckReasonCode(NanduReaderWriter(fd).read(op, token));

    if (op != ND_CHALLENGE) {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }

    Info("Got token %s.\n", token.c_str());

onExit:
    return code;
}

ReasonCode NanduClient::sendHash(int fd, const std::string& token) {
    NanduOperation op = ND_HASH;
    ReasonCode code;
    std::string hashStr = hash(token.c_str(), token.length());

    CheckReasonCode(NanduReaderWriter(fd).write(op, hashStr));
    Info("Hash send %s.\n", hashStr.c_str());
onExit:
    return code;
}

void NanduClient::push_(int fd, const Package& package) {
    std::string token;
    ReasonCode code;
    Package taskPkg;
    Task task("fuck");
    task.addParam("1");
    task.addParam("2");
    task.addParam("xxx");
    NanduOperation op = ND_PUSH;

    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));

    CheckReasonCode(task.package(taskPkg));
    CheckReasonCode(NanduReaderWriter(fd).write(op, taskPkg));

    Info("Push task %s\n", task.getName().c_str());
onExit:
    return;
}

void NanduClient::pop_(int fd, Package& package) {
    std::string token;
    ReasonCode code;
    Package taskPkg;
    Task task;
    NanduOperation op = ND_POP;

    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));
    CheckReasonCode(NanduReaderWriter(fd).write(op, std::string("")));

    CheckReasonCode(NanduReaderWriter(fd).read(op, taskPkg));
    if (op != ND_POP) {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }
    CheckReasonCode(CreateTaskFromPackage(taskPkg, task));

    Info("Pop task %s\n", task.getName().c_str());
onExit:
    return;
}
