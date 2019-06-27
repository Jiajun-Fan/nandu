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

struct Package {
    PackageReasonCode code;
    std::vector<unsigned char> bytes;
};

class PackageReaderWriter {
public:
    explicit PackageReaderWriter(int fd);
    virtual ~PackageReaderWriter();
    std::unique_ptr<Package> readPackage();
    size_t writePackage();
private:
    PackageReasonCode read(unsigned char* buff, size_t toRead);
    int                             _fd;
};
