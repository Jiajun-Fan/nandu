#include "ND_server.hh"
#include <limits.h>
#include <unistd.h>

NanduServer::NanduServer(bool local, int port, 
            ConnectionFunctor functor, const std::string& key) :
            Signer(key), Server(local, port, functor) {
}

NanduServer::~NanduServer() {
}

static void* handler(Connection* conn) {
    while (1) {
        char buff[PATH_MAX];
        int num = read(conn->fd, buff, sizeof(buff));
        if (num > 0) {
            write(2, buff, num);
        }
    }
}

int main() {
    NanduServer* server = new NanduServer(false, 6161, handler, "password");
    server->run();
    return 0;
}
