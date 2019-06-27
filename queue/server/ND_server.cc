#include "ND_server.hh"
#include <limits.h>
#include <unistd.h>
#include "package.hh"
#include "error.hh"

NanduServer::NanduServer(bool local, int port, 
            ConnectionHandler hd, const std::string& key) :
            Signer(key), Server(local, port, hd, this) {
}

NanduServer::~NanduServer() {
}

static void* handleConnection(Connection* conn) {
    NanduServer* server = (NanduServer*)conn->serverArgs;

    std::unique_ptr<Package> package = PackageReaderWriter(conn->fd).readPackage();
    if (package->code != PKG_OK) {
        if (package->code == PKG_CORRUPTED) {
            Warning() << "got corrupted package." << std::endl;
        } else {
            Warning() << "lost connection." << std::endl;
        }
        return NULL;
    }
}

int main() {
    NanduServer* server = new NanduServer(false, 6161, handleConnection, "password");
    server->run();
    return 0;
}
