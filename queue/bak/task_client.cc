#include "task_client.hh"
#include <string.h>
#include "log.hh"
#include "packaged_task.hh"

ReasonCode TaskClient::waitForChallenge(int fd, std::string& token) {
    Operation op;
    ReasonCode code;
    CheckReasonCode(OperationReaderWriter(fd).read(op, token));

    if (op != OP_CHALLENGE) {
        Debug("Got opcode %d.\n", op);
        CheckReasonCode(RC_OP_WRONG_CODE);
    }

    Info("Got token %s.\n", token.c_str());

onExit:
    return code;
}

ReasonCode TaskClient::sendHash(int fd, const std::string& token) {
    Operation op = OP_HASH;
    ReasonCode code;
    std::string hashStr = hash(token.c_str(), token.length());

    CheckReasonCode(OperationReaderWriter(fd).write(op, hashStr));
    Info("Hash send %s.\n", hashStr.c_str());

    CheckReasonCode(OperationReaderWriter(fd).read(op, hashStr));
    if (op != OP_VALIDATED) {
        CheckReasonCode(RC_AUTH_BAD_HASH);
    }
    Info("Hash validated %s.\n", hashStr.c_str());
onExit:
    return code;
}

void TaskClient::push_(int fd, Package* package) {
    std::string token;
    ReasonCode code;
    PackagedTask* task = dynamic_cast<PackagedTask*>(package);
    Operation op = OP_PUSH;

    CheckReasonCode(task->toPackage());
    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));

    CheckReasonCode(OperationReaderWriter(fd).write(op, *task));
    Info("push task.\n");
    task->printTask();
onExit:
    printError(code);
    return;
}

void TaskClient::pop_(int fd, Package* package) {
    std::string token;
    ReasonCode code;
    PackagedTask* task = dynamic_cast<PackagedTask*>(package);
    Operation op = OP_POP;

    CheckReasonCode(waitForChallenge(fd, token));
    CheckReasonCode(sendHash(fd, token));
    CheckReasonCode(OperationReaderWriter(fd).write(op, std::string("")));

    CheckReasonCode(OperationReaderWriter(fd).read(op, *task));
    if (op != OP_POP) {
        Debug("Got opcode %d.\n", op);
        CheckReasonCode(RC_OP_WRONG_CODE);
    }
    CheckReasonCode(task->toTask());
    Info("pop task.\n");
    task->printTask();
onExit:
    printError(code);
    return;
}
