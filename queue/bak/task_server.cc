#include "task_server.hh"
#include "operation.hh"

ReasonCode TaskServer::handleConnection(int fd) {
    Operation op;
    ReasonCode code;
    Task task;
    Package taskPkg;
    CheckReasonCode(AuthServer::handleConnection(fd));
    CheckReasonCode(OperationReaderWriter(fd).read(op, taskPkg));

    if (op == OP_PUSH) {
        CheckReasonCode(CreateTaskFromPackage(taskPkg, task));
        pushTask((uint32_t)time(NULL), task);
        Info("Push task.\n", task.getName().c_str());
        task.printTask();
    } else if (op == OP_POP) {
        task = popTask();
        CheckReasonCode(task.package(taskPkg));
        CheckReasonCode(OperationReaderWriter(fd).write(OP_POP, taskPkg));
        Info("Pop task.\n", task.getName().c_str());
        task.printTask();
    } else {
        CheckReasonCode(RC_OP_WRONG_CODE);
    }

onExit:
    return code;
}

