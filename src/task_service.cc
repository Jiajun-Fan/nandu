#include "task_service.hh"
#include <assert.h>
#include "log.hh"
#include "exception.hh"

enum {
    SC_TASK_PUSH_WAIT_OK,
    SC_TASK_POP_WAIT_OK,
};

static const int kOperationTaskPush   = OperationCode(SVC_TASK, SUB_TASK_PUSH).getOpCode();
static const int kOperationTaskPop    = OperationCode(SVC_TASK, SUB_TASK_POP).getOpCode();
static const int kOperationTaskPushOK = OperationCode(SVC_TASK, SUB_TASK_PUSH_OK).getOpCode();
static const int kOperationTaskPopOK  = OperationCode(SVC_TASK, SUB_TASK_POP_OK).getOpCode();

TaskServerService::TaskServerService() {
    _entryMap[kOperationTaskPush] = OperationEntry(handleTaskPush, SC_INIT);
    _entryMap[kOperationTaskPop] = OperationEntry(handleTaskPop, SC_INIT);
}

void TaskServerService::handleTaskPush(Service* service, Session& session, const Operation& in) {
    TaskServerService* svc = dynamic_cast<TaskServerService*>(service);
    assert(svc);

    Task task = in.toTask();
    svc->pushTask((uint32_t)time(NULL), task);
    Operation(kOperationTaskPushOK).write(session.fd);
    Info("S: Push task \"%s\".\n", task.getName().c_str());
};

void TaskServerService::handleTaskPop(Service* service, Session& session, const Operation& in) {
    TaskServerService* svc = dynamic_cast<TaskServerService*>(service);
    assert(svc);

    Task task = svc->popTask();
    Operation out(kOperationTaskPushOK);
    out.fromTask(task);
    out.write(session.fd);
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
    // no need to update session.curState, since it's still SC_INIT
};

TaskClientService::TaskClientService() {
    _entryMap[kOperationTaskPush] = OperationEntry(handleTaskPushPop, SC_INIT);
    _entryMap[kOperationTaskPop] = OperationEntry(handleTaskPushPop, SC_INIT);
    _entryMap[kOperationTaskPushOK] = OperationEntry(handleTaskPushOK, SC_TASK_PUSH_WAIT_OK);
    _entryMap[kOperationTaskPopOK] = OperationEntry(handleTaskPopOK, SC_TASK_POP_WAIT_OK);
}

void TaskClientService::handleTaskPushPop(Service* service, Session& session, const Operation& in) {
    if (in.getOpCode() == kOperationTaskPush) {
        session.curState = SC_TASK_PUSH_WAIT_OK;
    } else if (in.getOpCode() == kOperationTaskPop) {
        session.curState = SC_TASK_POP_WAIT_OK;
    }
    in.write(session.fd);
};

void TaskClientService::handleTaskPushOK(Service* service, Session& session, const Operation& in) {
    session.curState = SC_INIT;
    Info("C: Task pushed.\n");
}

void TaskClientService::handleTaskPopOK(Service* service, Session& session, const Operation& in) {
    Task task = in.toTask();
    session.curState = SC_INIT;
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
}
