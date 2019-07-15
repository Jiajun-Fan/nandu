#pragma once
#include "service.hh"
#include "queue.hh"

class TaskEnum {
private:
enum {
    OP_TASK_PUSH,
    OP_TASK_POP,
    OP_TASK_PUSH_OK,
    OP_TASK_POP_OK,
};
enum {
    SC_TASK_PUSH_WAIT_OK,
    SC_TASK_POP_WAIT_OK,
};
public:
    static const int kOpPush;
    static const int kOpPop;
    static const int kOpPushOK;
    static const int kOpPopOK;
    static const int kScPushWaitOK;
    static const int kScPopWaitOK;
};

class TaskServerService : public Service, public Queue, public TaskEnum {
public:
    int getServiceCode() const { return SVC_TASK; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    TaskServerService();
    virtual ~TaskServerService() {}
private:
    static void handleTaskPush(Service* service, Session& session, const Operation& in, Operation& out);
    static void handleTaskPop(Service* service, Session& session, const Operation& in, Operation& out);

    EntryMap                            _entryMap;
};

class TaskClientService : public Service, public TaskEnum {
public:
    int getServiceCode() const { return SVC_TASK; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    TaskClientService();
    virtual ~TaskClientService() {}
private:
    static void handleTaskPushPop(Service* service, Session& session, const Operation& in, Operation& out);
    static void handleTaskPushOK(Service* service, Session& session, const Operation& in, Operation& out);
    static void handleTaskPopOK(Service* service, Session& session, const Operation& in, Operation& out);
    EntryMap                            _entryMap;
};
