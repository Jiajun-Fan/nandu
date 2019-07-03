#include "ND_server.hh"
#include "log.hh"
int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    NanduServer server(kRemoteServer, 6161, "password");
    server.run();
    return 0;
}
