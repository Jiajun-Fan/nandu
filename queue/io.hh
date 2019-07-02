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
    const unsigned char* data() const noexcept { return _data.data(); }
    unsigned char* data() noexcept { return _data.data(); }
    void resize(std::vector<unsigned char>::size_type size) { _data.resize(size); }
    std::vector<unsigned char>::size_type size() const noexcept { return _data.size(); }
    void push_back(unsigned char c) { _data.push_back(c); }
    virtual ~Package() {}
    
private:
    std::vector<unsigned char>      _data;
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
