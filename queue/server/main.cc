#include "ND_server.hh"
int main() {
    NanduServer server(kRemoteServer, 6161, "password");
    server.run();
    return 0;
}
