#pragma once
#include "service.hh"
#include "queue.hh"

enum {
    SUB_TASK_PUSH,
    SUB_TASK_POP,
    SUB_TASK_PUSH_OK,
    SUB_TASK_POP_OK,
};

class TaskServerService : public Service, public Queue {
public:
    int getServiceCode() const { return SVC_TASK; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    TaskServerService();
    virtual ~TaskServerService() {}
private:
    static void handleTaskPush(Service* service, Session& session, const Operation& in);
    static void handleTaskPop(Service* service, Session& session, const Operation& in);

    EntryMap                            _entryMap;
};

class TaskClientService : public Service {
public:
    int getServiceCode() const { return SVC_TASK; }
    virtual const EntryMap& getEntryMap() const { return _entryMap; }

    TaskClientService();
    virtual ~TaskClientService() {}
private:
    static void handleTaskPushPop(Service* service, Session& session, const Operation& in);
    static void handleTaskPushOK(Service* service, Session& session, const Operation& in);
    static void handleTaskPopOK(Service* service, Session& session, const Operation& in);
    EntryMap                            _entryMap;
};
