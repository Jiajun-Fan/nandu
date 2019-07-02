#pragma once
#include <sys/types.h>
#include <vector>
#include <string>
#include "reason_code.hh"

struct RawPackageHead {
    char magic[5];
    size_t size;
};

typedef std::vector<unsigned char> Package;

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
