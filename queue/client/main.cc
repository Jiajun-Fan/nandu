#include "ND_client.hh"
#include "log.hh"
int main() {
    SetLogLevel(kLogDebug);
    NanduClient client("127.0.0.1", 6161, "password");
    Package pkg;
    client.push(pkg);
    client.pop(pkg);
    return 0;
}
