#pragma once
#include <sys/types.h>
#include <vector>
#include <string>
#include "reason_code.hh"

struct RawPackageHead {
    char magic[5];
    size_t size;
};

class Package {
public:
    virtual const unsigned char* cData() const = 0;
    virtual unsigned char* data() = 0;
    virtual void resize(size_t size) = 0;
    virtual size_t size() const = 0;
    virtual ~Package() {}
};

class PackageReaderWriter {
public:
    explicit PackageReaderWriter(int fd);
    virtual ~PackageReaderWriter();
    ReasonCode readPackage(Package& package);
    ReasonCode writePackage(const Package& package);

private:
    ReasonCode read(unsigned char* buff, size_t toRead);
    ReasonCode write(const unsigned char* buff, size_t toWrite);
    void printError(ReasonCode code);
    int                             _fd;
};
