#include "server.hh"
#include "auth.hh"
#include "log.hh"
int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Server* server = MakeServer(kRemoteServer, 6161);

    server->registerService(new AuthServerService("password"));
    server->run();

    delete server;
    return 0;
}
