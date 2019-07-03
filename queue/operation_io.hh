#pragma once
#include "io.hh"
#include <string.h>

typedef enum {
    OP_CHALLENGE = 0,
    OP_HASH,
    OP_VALIDATED,
    OP_BAD_HASH,
    OP_PUSH,
    OP_POP,
    OP_BAD_OPT
} Operation;

class OperationReaderWriter : public PackageReaderWriter {
public:
    explicit OperationReaderWriter(int fd) : PackageReaderWriter(fd) {}
    virtual ~OperationReaderWriter() {}

    ReasonCode read(Operation& op, Package& package);
    ReasonCode write(const Operation& op, const Package& package);
    ReasonCode read(Operation& op, std::string& msg);
    ReasonCode write(const Operation& op, const std::string& msg);
};
