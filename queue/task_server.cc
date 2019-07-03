#include "task_server.hh"
#include "operation_io.hh"

ReasonCode TaskServer::handleOperation(Connection* conn) {
    int fd = conn->fd;
    Operation op;
    ReasonCode code;
    Task task;
    Package taskPkg;
    CheckReasonCode(OperationReaderWriter(fd).read(op, taskPkg));

    if (op == ND_PUSH) {
        CheckReasonCode(CreateTaskFromPackage(taskPkg, task));
        pushTask((uint32_t)time(NULL), task);
        Info("Push task.\n", task.getName().c_str());
        task.printTask();
    } else if (op == ND_POP) {
        task = popTask();
        CheckReasonCode(task.package(taskPkg));
        CheckReasonCode(OperationReaderWriter(fd).write(ND_POP, taskPkg));
        Info("Pop task.\n", task.getName().c_str());
        task.printTask();
    } else {
        CheckReasonCode(RC_ND_WRONG_CODE);
    }

onExit:
    return code;
}

