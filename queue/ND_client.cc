#include "ND_client.hh"
#include <string.h>
#include "log.hh"
#include "ND_task.hh"

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

    CheckReasonCode(NanduReaderWriter(fd).read(op, hashStr));
    if (op != ND_VALIDATED) {
        CheckReasonCode(RC_ND_BAD_HASH);
    }
    Info("Hash validated %s.\n", hashStr.c_str());
onExit:
    return code;
}

void NanduClient::push_(int fd, Package* package) {
    std::string token;
    ReasonCode code;
    NanduTask* task = dynamic_cast<NanduTask*>(package);
    NanduOperation op = ND_PUSH;

    CheckReasonCode(task->toPackage());
    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));

    CheckReasonCode(NanduReaderWriter(fd).write(op, *task));
    Info("push task.\n");
    task->printTask();
onExit:
    printError(code);
    return;
}

void NanduClient::pop_(int fd, Package* package) {
    std::string token;
    ReasonCode code;
    NanduTask* task = dynamic_cast<NanduTask*>(package);
    NanduOperation op = ND_POP;

    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));
    CheckReasonCode(NanduReaderWriter(fd).write(op, std::string("")));

    CheckReasonCode(NanduReaderWriter(fd).read(op, *task));
    if (op != ND_POP) {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }
    CheckReasonCode(task->toTask());
    Info("pop task.\n");
    task->printTask();
onExit:
    printError(code);
    return;
}
