#pragma once
#include "io.hh"
#include <string.h>

typedef enum {
    ND_CHALLENGE = 0,
    ND_HASH,
    ND_PUSH,
    ND_POP,
    ND_BAD_OPT
} NanduOperation;

class NanduReaderWriter : public PackageReaderWriter {
public:
    explicit NanduReaderWriter(int fd) : PackageReaderWriter(fd) {}
    virtual ~NanduReaderWriter() {}

    PackageReasonCode read(NanduOperation& op, Package& package);
    PackageReasonCode write(const NanduOperation& op, Package& package);
    PackageReasonCode read(NanduOperation& op, std::string& msg);
    PackageReasonCode write(const NanduOperation& op, const std::string& msg);
};
