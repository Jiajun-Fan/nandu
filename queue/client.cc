#include "client.hh"
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "log.hh"

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

ReasonCode Client::start(Session& session) {
    ReasonCode code;
    std::string doneMsg;
    Operation auth, out, in;
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockFd > 0);

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = _addr;
    servAddr.sin_port = htons(_port);

    if (::connect(sockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 0) {
        CheckReasonCode(RC_IO_CANNOT_CONNECT);
    }

    session.fd = sockFd;
    session.curState = C_INIT;
    session.data = "";
    session.send = false;

    CheckReasonCode(auth.read(sockFd));
    if (auth.opCode() != OP_AUTH_INIT) {
        Debug("opcode %d\n", auth.opCode());
        CheckReasonCode(RC_OP_WRONG_CODE);
    }
    if (auth.getData()->size() != 0) {
        if (hasAuth()) {
            session.curState = C_AUTH_INIT;
            CheckReasonCode(runOperation(session, auth));
        } else {
            out.setOpCode(OP_DONE);
            CheckReasonCode(out.write(sockFd));
            Debug("opcode %d\n", auth.opCode());
            CheckReasonCode(RC_OP_UNSUPPORTED);
        }
    }
    while (session.curState != C_INIT) {
        CheckReasonCode(in.read(session.fd));
        if (in.opCode() == OP_DONE) {
            CheckReasonCode(Operation2String(in, doneMsg));
            Debug("%s\n", doneMsg.c_str());
            CheckReasonCode(RC_CLOSED_BY_SERVER);
        }
        CheckReasonCode(runOperation(session, in));
    }

onExit:
    if (code != RC_OK) {
        close(sockFd);
    }
    printError(code);
}

ReasonCode Client::run(Session& session, const Operation& operation) {
    ReasonCode code;
    Operation in;
    std::string doneMsg;
    CheckReasonCode(runOperation(session, operation));
    while (session.curState != C_INIT) {
        CheckReasonCode(in.read(session.fd))
        if (in.opCode() == OP_DONE) {
            CheckReasonCode(Operation2String(in, doneMsg));
            Debug("%s\n", doneMsg.c_str());
            CheckReasonCode(RC_CLOSED_BY_SERVER);
        }
        CheckReasonCode(runOperation(session, in));
    }
onExit:
    if (code != RC_OK) {
        close(session.fd);
    }
    printError(code);
}

ReasonCode Client::end(Session& session) {
    ReasonCode code;
    Operation out;
    out.setOpCode(OP_DONE);
    CheckReasonCode(out.write(session.fd));
onExit:
    close(session.fd);
    printError(code);
}
