#include "client.hh"
#include "auth.hh"
#include "log.hh"

int main() {
#ifndef NDEBUG
    SetLogLevel(kLogDebug);
#endif
    Client client("127.0.0.1", 6161);
    client.registerService(new AuthClientService("password"));
    Session session = { 0, C_INIT, "", false};
    client.start(session);
    client.end(session);

    return 0;
}
