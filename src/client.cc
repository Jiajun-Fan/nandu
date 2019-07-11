#include "client.hh"
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "log.hh"
#include "exception.hh"

Client::Client(const std::string& host, int port) :
                _port(port) {
    _addr = inet_addr(host.c_str());
    if (_addr < 0) {
        Fatal("Failed to get ip of \"%s\".\n", host.c_str());
        exit(-1);
    }
}

Client::~Client() {
}

int Client::connect() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockFd > 0);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = _addr;
    servAddr.sin_port = htons(_port);

    if (::connect(sockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 0) {
        throw Exception(RC_IO_CANNOT_CONNECT);
    }
    return sockFd;
}

Session Client::start() {

    Session session = { this->connect(), C_INIT, "", false };

    Operation auth;
    auth.read(session.fd);

    if (auth.opCode() != OP_AUTH_INIT) {
        Debug("opcode %d\n", auth.opCode());
        throw Exception(RC_OP_WRONG_CODE);
    }

    if (auth.getData().size() != 0) {
        if (hasAuth()) {
            session.curState = C_AUTH_INIT;
            runOperation(session, auth);
        } else {
            // not able to auth, abort
            Operation(OP_DONE).write(session.fd);
            Debug("opcode %d\n", auth.opCode());
            throw Exception(RC_OP_UNSUPPORTED);
        }
    }

    while (session.curState != C_INIT) {
        Operation in;
        in.read(session.fd);
        if (in.opCode() == OP_DONE) {
            std::string doneMsg;
            Package2String(in.getCdata(), doneMsg);
            Debug("%s\n", doneMsg.c_str());
            throw Exception(RC_SERVER_CLOSED);
        }
        runOperation(session, in);
    }

    return session;
}

void Client::run(Session& session, const Operation& operation) {
    runOperation(session, operation);
    while (session.curState != C_INIT) {
        Operation in;
        in.read(session.fd);
        if (in.opCode() == OP_DONE) {
            std::string doneMsg;
            Package2String(in.getCdata(), doneMsg);
            Debug("%s\n", doneMsg.c_str());
            throw Exception(RC_SERVER_CLOSED);
        }
        runOperation(session, in);
    }
}

void Client::end(Session& session) {
    Operation(OP_DONE).write(session.fd);
    close(session.fd);
}
