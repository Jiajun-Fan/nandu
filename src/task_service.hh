#pragma once
#include "service.hh"
#include "queue.hh"

class TaskServerService : public Service, public Queue {
public:
    virtual void handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    TaskServerService() {}
    virtual ~TaskServerService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    void handleTaskPush(Session& session, const Operation& in, Operation& out);
    void handleTaskPop(Session& session, const Operation& in, Operation& out);
    static const OperationCode _operations[];
};

class TaskClientService : public Service {
public:
    virtual void handleOperation(OperationCode op, Session& session,
                           const Operation& in, Operation& out);

    TaskClientService() {}
    virtual ~TaskClientService() {}
    virtual const OperationCode* getOperations() const { return _operations; }
private:
    void handleTaskPushPop(Session& session, const Operation& in, Operation& out);
    void handleTaskPushOK(Session& session, const Operation& in, Operation& out);
    void handleTaskPopOK(Session& session, const Operation& in, Operation& out);
    static const OperationCode _operations[];
};
