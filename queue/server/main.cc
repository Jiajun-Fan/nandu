#include "server.hh"
#include "auth.hh"
#include "log.hh"
int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Server server(kRemoteServer, 6161);

    //server.registerService(new AuthServerService("password"));
    server.run();

    return 0;
}
