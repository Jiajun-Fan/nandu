#include "ND_client.hh"
int main() {
    NanduClient client("127.0.0.1", 6161, "password");
    client.connect();
    return 0;
}
