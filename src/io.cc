#include "io.hh"
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "log.hh"
#include "exception.hh"

const char* kPackageMagic = "nandu";
const size_t kMaxPackageSize = (4096 * 4096); // 16 MB

PackageReaderWriter::PackageReaderWriter(int fd) : _fd(fd) {
}

PackageReaderWriter::~PackageReaderWriter() {
}

void PackageReaderWriter::read(unsigned char* buff, size_t toRead) {
    while (toRead != 0) {
        ssize_t nbRead = ::read(_fd, buff, toRead);
        if (nbRead < 0) {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            case ECONNRESET:
                throw Exception(RC_IO_CONN_RESET);
            default:
                throw Exception(RC_IO_UNKNOWN);
            }
        } else if (nbRead == 0) {
            throw Exception(RC_IO_FD_CLOSED);
        } else {
            assert(toRead >= nbRead);
            toRead -= nbRead;
            buff += nbRead;
        }
    }
}

void PackageReaderWriter::readPackage(Package& package) {

    RawPackageHead head;

    read((unsigned char*)&head, sizeof(head));

    if (strncmp(head.magic, kPackageMagic, strlen(kPackageMagic))) {
        Debug("Got magic \"%c%c%c%c%c\".\n", head.magic[0], head.magic[1], 
                head.magic[2], head.magic[3], head.magic[4]);
        throw Exception(RC_IO_PKG_CORRUPTED);
    } else if(head.size > kMaxPackageSize) {
        Debug("Got package size %lu.\n", (unsigned long)head.size);
        throw Exception(RC_IO_PKG_CORRUPTED);
    } else {
        package.resize(head.size);
        read(package.data(), head.size);
    }
}

void PackageReaderWriter::write(const unsigned char* buff, size_t toWrite) {
    while (toWrite != 0) {
        ssize_t nbWrite = ::write(_fd, buff, toWrite);
        if (nbWrite < 0) {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            default:
                assert(0);
            }
        } else if (nbWrite == 0) {
            throw Exception(RC_IO_FD_CLOSED);
        } else {
            assert(toWrite >= nbWrite);
            toWrite -= nbWrite;
            buff += nbWrite;
        }
    }
}

void PackageReaderWriter::writePackage(const Package& package) {
    RawPackageHead head;

    size_t size = package.size();
    memcpy(head.magic, kPackageMagic, strlen(kPackageMagic));
    memcpy(&head.size, &size, sizeof(size));

    write((unsigned char*)&head, sizeof(head));
    write(package.cData(), head.size);
}

std::string Package::toString() const {
    const unsigned char* buff = cData();
    size_t s = size();
    if (s == 0) {
        return std::string("");
    } else if (buff[s-1] != '\0') {
        throw Exception(RC_OP_DATA_NOTSTRING);
    } else {
        return std::string((const char*)buff);
    }
}

void Package::fromString(const std::string& str) {
    size_t size = str.length() + 1;
    resize(size);
    memcpy(data(), str.c_str(), size);
}

static inline ssize_t getStringLength(const unsigned char* buff) {
    for (ssize_t i = 0; i < kTaskMaxStringLength; i++) {
        if (buff[i] == 0) {
            return i+1;
        }
    }
    return -1;
}

static void readString(const unsigned char* buff, unsigned char*& ptr, std::string& str) {
    ssize_t strLength = getStringLength(buff);
    if (strLength < 0) {
        throw Exception(RC_TSK_STRING_TOO_LONG);
    }
    ptr = (unsigned char*)buff + strLength;
    str = std::string((const char*)buff);
}

static void writeString(unsigned char* buff, unsigned char*& ptr, std::string str) {
    size_t strLength = str.length()+1;
    if (strLength > kTaskMaxStringLength) {
        throw Exception(RC_TSK_STRING_TOO_LONG);
    }
    memcpy(buff, str.c_str(), strLength);
    ptr = (unsigned char*)buff + strLength;
}

Task Package::toTask() const {

    Task task;
    std::string name;

    unsigned char* ptr = (unsigned char*)cData();
    size_t nbParam = 0;
    readString(ptr, ptr, name);
    task.setName(name);

    nbParam = *(size_t*)(ptr);
    if (nbParam > kTaskMaxParamNum) {
        throw Exception(RC_TSK_TOOMANY_PARAMS);
    }
    ptr += sizeof(size_t);

    for (unsigned i = 0; i < nbParam; i++) {
        std::string param;
        readString(ptr, ptr, param);
        task.addParam(param);
    }
    return task;
}

void Package::fromTask(const Task& task) {

    // size of name
    size_t totalSize = task.getName().length() + 1;
    unsigned char* ptr = NULL;

    // number of parameters 
    if (task.getParams().size() > kTaskMaxParamNum) {
        throw Exception(RC_TSK_TOOMANY_PARAMS);
    }
    totalSize += sizeof(size_t);

    for (auto it = task.getParams().begin();
            it != task.getParams().end(); it++) {
        totalSize += (it->length() + 1);
    }
    resize(totalSize);

    ptr = data();
    writeString(ptr, ptr, task.getName());
    *(size_t*)(ptr) = task.getParams().size();
    ptr += sizeof(size_t);

    for (auto it = task.getParams().begin();
            it != task.getParams().end(); it++) {
        writeString(ptr, ptr, *it);
    }
}
