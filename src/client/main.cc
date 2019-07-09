#include "client.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "task.hh"
#include "log.hh"

int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Client client("127.0.0.1", 6161);
    client.registerService(new AuthClientService("password"));
    client.registerService(new TaskClientService());
    Session session = { 0, C_INIT, "", false};
    ReasonCode code;

    Task task("fuck");
    Operation opPush;
    Operation opPop;
    opPush.setOpCode(OP_TASK_PUSH);
    opPop.setOpCode(OP_TASK_POP);
    CheckReasonCode(Task2Package(task, opPush.getData()));
    CheckReasonCode(client.start(session));
    CheckReasonCode(client.run(session, opPush));
    CheckReasonCode(client.run(session, opPop));
    CheckReasonCode(client.run(session, opPop));

    client.end(session);

onExit:
    return 0;
}
