#include "task_service.hh"
#include "log.hh"
#include "exception.hh"
void TaskServerService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_TASK_PUSH:
        handleTaskPush(session, in, out);
        break;
    case OP_TASK_POP:
        handleTaskPop(session, in, out);
        break;
    default:
        assert(0);
    }
}

const OperationCode TaskServerService::_operations[] = {
    OP_TASK_PUSH,
    OP_TASK_POP,
    OP_BAD_OPERATION,
};

void TaskServerService::handleTaskPush(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_INIT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }

    Task task;
    Package2Task(in.getCdata(), task);
    pushTask((uint32_t)time(NULL), task);
    out.setOpCode(OP_TASK_PUSH_OK);
    session.send = true;
    Info("S: Push task \"%s\".\n", task.getName().c_str());
};

void TaskServerService::handleTaskPop(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_INIT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }

    Task task = popTask();
    Task2Package(task, out.getData());
    out.setOpCode(OP_TASK_POP_OK);
    session.send = true;
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
};

void TaskClientService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_TASK_PUSH:
    case OP_TASK_POP:
        handleTaskPushPop(session, in, out);
        break;
    case OP_TASK_PUSH_OK:
        handleTaskPushOK(session, in, out);
        break;
    case OP_TASK_POP_OK:
        handleTaskPopOK(session, in, out);
        break;
    default:
        assert(0);
    }
}

const OperationCode TaskClientService::_operations[] = {
    OP_TASK_PUSH,
    OP_TASK_POP,
    OP_TASK_PUSH_OK,
    OP_TASK_POP_OK,
    OP_BAD_OPERATION,
};

void TaskClientService::handleTaskPushPop(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_INIT) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }
    if (in.opCode() == OP_TASK_PUSH) {
        session.curState = C_TASK_PUSH;
    } else if (in.opCode() == OP_TASK_POP) {
        session.curState = C_TASK_POP;
    }

    out = in; 
    session.send = true;
};

void TaskClientService::handleTaskPushOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_TASK_PUSH) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }

    session.curState = C_INIT;
    Info("C: Task pushed.\n");
}

void TaskClientService::handleTaskPopOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_TASK_POP) {
        Debug("state %d\n", session.curState); 
        throw Exception(RC_OP_WRONG_CODE);
    }

    Task task;
    Package2Task(in.getCdata(), task);

    session.curState = C_INIT;
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
}
