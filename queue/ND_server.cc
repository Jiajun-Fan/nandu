#include "ND_server.hh"
#include <limits.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <sstream>
#include "error.hh"


NanduServer::NanduServer(bool local, int port, 
            const std::string& key) :
            Signer(key), Server(local, port) {
}

NanduServer::~NanduServer() {
}

std::string NanduServer::generateToken() {

    static bool inited = false;
    if (!inited) {
        srandom((unsigned)time(NULL)*356821);
        inited = true;
    }
    
    std::stringstream ss;
    long r = random();
    unsigned char* p = (unsigned char*)&r;
    for (unsigned i = 0; i < sizeof(long); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)(p[i]);
    }
    return ss.str();
}

bool NanduServer::sendChallenge(int fd, const std::string& token) {
    NanduOperation op = OpChallenge;
    Package package;
    package.resize(token.length() + sizeof(op));
    memcpy(package.data(), &op, sizeof(op));
    memcpy(package.data()+sizeof(op), token.c_str(), token.length());

    PackageReasonCode code = PackageReaderWriter(fd).writePackage(package);
    if (code != PKG_OK) {
        Error() << "Error when reading package " << code << "." << std::endl;
        return false;
    }
    Info() << "Token send " << token << "." << std::endl;
    return true;
}

bool NanduServer::waitForHash(int fd, const std::string& token) {
    Package package;
    PackageReasonCode code = PackageReaderWriter(fd).readPackage(package);
    if (code != PKG_OK) {
        Error() << "Error when reading package " << code << "." << std::endl;
        return false;
    }

    NanduOperation op = *(NanduOperation*)package.data();
    if (op != OpHash) {
        Error() << "Get wrong opcode" << op << "." << std::endl;
        return false;
    }

    std::string s = sign(token.c_str(), token.length());
    if (strncmp(s.c_str(), ((char*)package.data())+sizeof(op), token.length())) {
        Error() << "Bad hash " << s << std::endl;
        return false;
    }
    Info() << "Hash validated " << s << "." << std::endl;
    return true;
}

void NanduServer::handleConnection(Connection* conn) {
    std::string token = generateToken();
    if (!sendChallenge(conn->fd, token) ||
            !waitForHash(conn->fd, token)) {
        Error() << "Bad request." << std::endl;
        return;
    }
}

