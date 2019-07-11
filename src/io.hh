#pragma once
#include <sys/types.h>
#include <vector>
#include <string>

struct RawPackageHead {
    char magic[5];
    size_t size;
};

class Task;

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
    void readPackage(Package& package);
    void writePackage(const Package& package);

private:
    void read(unsigned char* buff, size_t toRead);
    void write(const unsigned char* buff, size_t toWrite);
    int                             _fd;
};

extern void Package2String(const Package& package, std::string& str);
extern void String2Package(const std::string& str, Package& package);
extern void Task2Package(const Task& task, Package& package);
extern void Package2Task(const Package& package, Task& task);
