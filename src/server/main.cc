#include "server.hh"
#include "auth_service.hh"
#include "task_service.hh"
#include "log.hh"
int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Server* server = MakeServer(kRemoteServer, 6161);

    server->registerService(new AuthServerService("password"));
    server->registerService(new TaskServerService());
    server->run();

    delete server;
    return 0;
}
