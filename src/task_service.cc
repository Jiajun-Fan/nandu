#include "task_service.hh"
#include <assert.h>
#include "log.hh"
#include "exception.hh"

const int TaskEnum::kOpPush       = genOperationCode(SVC_TASK, OP_TASK_PUSH);     // 512
const int TaskEnum::kOpPop        = genOperationCode(SVC_TASK, OP_TASK_POP);      // 513
const int TaskEnum::kOpPushOK     = genOperationCode(SVC_TASK, OP_TASK_PUSH_OK);  // 514
const int TaskEnum::kOpPopOK      = genOperationCode(SVC_TASK, OP_TASK_POP_OK);   // 515
const int TaskEnum::kScPushWaitOK = genStateCode(SVC_TASK, SC_TASK_PUSH_WAIT_OK); // 512
const int TaskEnum::kScPopWaitOK  = genStateCode(SVC_TASK, SC_TASK_POP_WAIT_OK);  // 513

TaskServerService::TaskServerService() {
    _entryMap[kOpPush] = OperationEntry(handleTaskPush, SC_INIT);
    _entryMap[kOpPop] = OperationEntry(handleTaskPop, SC_INIT);
}

void TaskServerService::handleTaskPush(Service* service, Session& session, const Operation& in) {
    TaskServerService* svc = dynamic_cast<TaskServerService*>(service);
    assert(svc);

    Task task = in.toTask();
    svc->pushTask((uint32_t)time(NULL), task);
    Operation(kOpPushOK).write(session.fd);
    Info("S: Push task \"%s\".\n", task.getName().c_str());
};

void TaskServerService::handleTaskPop(Service* service, Session& session, const Operation& in) {
    TaskServerService* svc = dynamic_cast<TaskServerService*>(service);
    assert(svc);

    Task task = svc->popTask();
    Operation out(kOpPopOK);
    out.fromTask(task);
    out.write(session.fd);
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
    // no need to update session.curState, since it's still SC_INIT
};

TaskClientService::TaskClientService() {
    _entryMap[kOpPush] = OperationEntry(handleTaskPushPop, SC_INIT);
    _entryMap[kOpPop] = OperationEntry(handleTaskPushPop, SC_INIT);
    _entryMap[kOpPushOK] = OperationEntry(handleTaskPushOK, kScPushWaitOK);
    _entryMap[kOpPopOK] = OperationEntry(handleTaskPopOK, kScPopWaitOK);
}

void TaskClientService::handleTaskPushPop(Service* service, Session& session, const Operation& in) {
    if (in.getOpCode() == kOpPush) {
        session.curState = kScPushWaitOK;
    } else if (in.getOpCode() == kOpPop) {
        session.curState = kScPopWaitOK;
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
    Info("C: Pop task \"%s\".\n", task.getName().c_str());
}
