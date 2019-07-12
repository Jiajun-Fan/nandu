#pragma once
#include <sys/types.h>
#include <vector>
#include <string>
#include "task.hh"

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
    virtual std::string toString() const;
    virtual void fromString(const std::string& str);
    virtual Task toTask() const;
    virtual void fromTask(const Task& task);
};

class PackageReaderWriter {
public:
    explicit PackageReaderWriter(int fd);
    virtual ~PackageReaderWriter();
    void readPackage(Package& package);
    void writePackage(const Package& package);

private:
    void read(unsigned char* buff, size_t toRead);
    void write(const unsigned char* buff, size_t toWrite);
    int                             _fd;
};
