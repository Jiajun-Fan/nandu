#pragma once
#include "service.hh"
#include "queue.hh"

class TaskServerService : public Service, public Queue {
public:
    virtual ReasonCode handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    TaskServerService() {}
    virtual ~TaskServerService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    ReasonCode handleTaskPush(Session& session, const Operation& in, Operation& out);
    ReasonCode handleTaskPop(Session& session, const Operation& in, Operation& out);
    static const OperationCode _operations[];
};

class TaskClientService : public Service {
public:
    virtual ReasonCode handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    TaskClientService() {}
    virtual ~TaskClientService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    ReasonCode handleTaskPushPop(Session& session, const Operation& in, Operation& out);
    ReasonCode handleTaskPushOK(Session& session, const Operation& in, Operation& out);
    ReasonCode handleTaskPopOK(Session& session, const Operation& in, Operation& out);
    static const OperationCode _operations[];
};
