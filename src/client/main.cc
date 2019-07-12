#include <unistd.h>
#include "client.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "task.hh"
#include "log.hh"
#include "exception.hh"

int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Client client("127.0.0.1", 6161);
    client.registerService(new AuthClientService("password"));
    client.registerService(new TaskClientService());
    Session session = { 0, SC_INIT, ""};

    Task task("fuck");

    try {
        Operation opPush(SVC_TASK, SUB_TASK_PUSH);
        opPush.fromTask(Task("fuck"));
        session = client.start();
        client.run(session, opPush);
        client.run(session, Operation(SVC_TASK, SUB_TASK_POP));
        client.run(session, Operation(SVC_TASK, SUB_TASK_POP));
        client.end(session);
    } catch (Exception e) {
        if (session.fd != 0) {
            close(session.fd);
        }
        Error("%s\n", e.what());
    }

    return 0;
}
