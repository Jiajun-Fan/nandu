#pragma once
#include <memory>
#include <vector>

typedef enum {
    PKG_OK = 0,
    PKG_CORRUPTED,
    PKG_FD_CLOSED,
} PackageReasonCode;

struct RawPackageHead {
    char magic[5];
    size_t size;
};

typedef std::vector<unsigned char> Package;

class PackageReaderWriter {
public:
    explicit PackageReaderWriter(int fd);
    virtual ~PackageReaderWriter();
    PackageReasonCode readPackage(Package& package);
    PackageReasonCode writePackage(Package& package);
private:
    PackageReasonCode read(unsigned char* buff, size_t toRead);
    PackageReasonCode write(unsigned char* buff, size_t toWrite);
    void printError(PackageReasonCode code);
    int                             _fd;
};
