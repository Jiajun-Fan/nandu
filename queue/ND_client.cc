#include "ND_client.hh"
#include <string.h>
#include "error.hh"

bool NanduClient::waitForChallenge(int fd, std::string& token) {
    Package package;
    PackageReasonCode code = PackageReaderWriter(fd).readPackage(package);
    if (code != PKG_OK) {
        Error() << "Error when reading package " << code << "." << std::endl;
        return false;
    }

    NanduOperation op = *(NanduOperation*)package.data();
    if (op != OpChallenge) {
        Error() << "Get wrong opcode" << op << "." << std::endl;
        return false;
    }

    package.push_back('\0');
    token = std::string((const char*)package.data()+sizeof(op));

    Info() << "Got token " << token << "." << std::endl;
    return true;
}

bool NanduClient::sendHash(int fd, const std::string& token) {
    NanduOperation op = OpHash;
    Package package;
    std::string hash = sign(token.c_str(), token.length());
    package.resize(hash.length() + sizeof(op));
    memcpy(package.data(), &op, sizeof(op));
    memcpy(package.data()+sizeof(op), hash.c_str(), hash.length());

    PackageReasonCode code = PackageReaderWriter(fd).writePackage(package);
    if (code != PKG_OK) {
        Error() << "Error when reading package " << code << "." << std::endl;
        return false;
    }
    Info() << "Hash send " << hash << "." << std::endl;
    return true;
}

void NanduClient::handleConnection(int fd) {
    std::string token;
    if (!waitForChallenge(fd, token) ||
            !sendHash(fd, token)) {
        Error() << "Failed to send request." << std::endl;
        return;
    }
}
