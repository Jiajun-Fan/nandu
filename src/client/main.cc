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
    ReasonCode code = RC_OK;

    Task task("fuck");
    Operation opPush(OP_TASK_PUSH);

    do {
        code = Task2Package(Task("fuck"), opPush.getData());
        code = client.start(session);
        code = client.run(session, opPush);
        code = client.run(session, Operation(OP_TASK_POP));
        code = client.run(session, Operation(OP_TASK_POP));
        code = client.end(session);
    } while(0);

    return 0;
}
