#include "task_service.hh"
ReasonCode TaskServerService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_TASK_PUSH:
        return handleTaskPush(session, in, out);
    case OP_TASK_POP:
        return handleTaskPop(session, in, out);
    default:
        assert(0);
    }
    return RC_OK;
}

const OperationCode TaskServerService::_operations[] = {
    OP_TASK_PUSH,
    OP_TASK_POP,
    OP_BAD_OPERATION,
};

ReasonCode TaskServerService::handleTaskPush(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_INIT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }

    ReasonCode code;
    Task task;
    CheckReasonCode(Package2Task(in.getCdata(), task));
    pushTask((uint32_t)time(NULL), task);
    out.setOpCode(OP_TASK_PUSH_OK);
    session.send = true;
    Info("S: Push task \"%s\".\n", task.getName().c_str());

onExit:
    return code;
};

ReasonCode TaskServerService::handleTaskPop(Session& session, const Operation& in, Operation& out) {
    if (session.curState != S_INIT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }

    ReasonCode code;
    Task task = popTask();
    CheckReasonCode(Task2Package(task, out.getData()));
    out.setOpCode(OP_TASK_POP_OK);
    session.send = true;
    Info("S: Pop task \"%s\".\n", task.getName().c_str());

onExit:
    return code;
};

ReasonCode TaskClientService::handleOperation(OperationCode op, Session& session,
        const Operation& in, Operation& out) {
    switch (op) {
    case OP_TASK_PUSH:
    case OP_TASK_POP:
        return handleTaskPushPop(session, in, out);
    case OP_TASK_PUSH_OK:
        return handleTaskPushOK(session, in, out);
    case OP_TASK_POP_OK:
        return handleTaskPopOK(session, in, out);
    default:
        assert(0);
    }
    return RC_OK;
}

const OperationCode TaskClientService::_operations[] = {
    OP_TASK_PUSH,
    OP_TASK_POP,
    OP_TASK_PUSH_OK,
    OP_TASK_POP_OK,
    OP_BAD_OPERATION,
};

ReasonCode TaskClientService::handleTaskPushPop(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_INIT) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }
    if (in.opCode() == OP_TASK_PUSH) {
        session.curState = C_TASK_PUSH;
    } else if (in.opCode() == OP_TASK_POP) {
        session.curState = C_TASK_POP;
    }

    out = in; 
    session.send = true;

    return RC_OK;
};

ReasonCode TaskClientService::handleTaskPushOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_TASK_PUSH) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }

    session.curState = C_INIT;
    Info("C: Task pushed.\n");

    return RC_OK;
}

ReasonCode TaskClientService::handleTaskPopOK(Session& session, const Operation& in, Operation& out) {
    if (session.curState != C_TASK_POP) {
        Debug("state %d\n", session.curState); 
        return RC_OP_WRONG_CODE;
    }

    ReasonCode code;
    Task task;
    CheckReasonCode(Package2Task(in.getCdata(), task));

    session.curState = C_INIT;
    Info("S: Pop task \"%s\".\n", task.getName().c_str());
onExit:
    return code;
}
