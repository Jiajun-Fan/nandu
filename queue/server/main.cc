#include "task_server.hh"
#include "log.hh"
int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    TaskServer server(kRemoteServer, 6161, "password");
    server.run();
    return 0;
}
