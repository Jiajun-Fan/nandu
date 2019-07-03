#pragma once
#include "io.hh"
#include <string.h>

typedef enum {
    ND_CHALLENGE = 0,
    ND_HASH,
    ND_VALIDATED,
    ND_BAD_HASH,
    ND_PUSH,
    ND_POP,
    ND_BAD_OPT
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
