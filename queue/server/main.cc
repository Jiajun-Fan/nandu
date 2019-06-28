#include "ND_server.hh"
#include "log.hh"
int main() {
    SetLogLevel(kLogDebug);
    NanduServer server(kRemoteServer, 6161, "password");
    server.run();
    return 0;
}
