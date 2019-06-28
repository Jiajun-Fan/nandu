#include "io.hh"
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "log.hh"

const char* kPackageMagic = "nandu";
const size_t kMaxPackageSize = (4096 * 4096); // 16 MB

PackageReaderWriter::PackageReaderWriter(int fd) : _fd(fd) {
}

PackageReaderWriter::~PackageReaderWriter() {
}

PackageReasonCode PackageReaderWriter::read(unsigned char* buff, size_t toRead) {
    while (toRead != 0) {
        ssize_t nbRead = ::read(_fd, buff, toRead);
        if (nbRead < 0) {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            default:
                assert(0);
            }
        } else if (nbRead == 0) {
            return PKG_FD_CLOSED;
        } else {
            assert(toRead >= nbRead);
            toRead -= nbRead;
            buff += nbRead;
        }
    }
    return PKG_OK;
}

PackageReasonCode PackageReaderWriter::readPackage(Package& package) {

    RawPackageHead head;
    PackageReasonCode code = read((unsigned char*)&head, sizeof(head));

    if (code == PKG_OK) {
        if (strncmp(head.magic, kPackageMagic, strlen(kPackageMagic))) {
            code = PKG_CORRUPTED;
        } else if(head.size > kMaxPackageSize) {
            code = PKG_CORRUPTED;
        } else {
            package.resize(head.size);
            code = read(package.data(), head.size);
        }
    }

    printError(code);
    return code;
}

PackageReasonCode PackageReaderWriter::write(unsigned char* buff, size_t toWrite) {
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
            return PKG_FD_CLOSED;
        } else {
            assert(toWrite >= nbWrite);
            toWrite -= nbWrite;
            buff += nbWrite;
        }
    }
    return PKG_OK;
}

PackageReasonCode PackageReaderWriter::writePackage(Package& package) {
    RawPackageHead head;
    size_t size = package.size();

    memcpy(head.magic, kPackageMagic, strlen(kPackageMagic));
    memcpy(&head.size, &size, sizeof(size));

    PackageReasonCode code = write((unsigned char*)&head, sizeof(head));

    if (code == PKG_OK) {
        code = write(package.data(), head.size);
    }

    printError(code);
    return code;
}

void PackageReaderWriter::printError(PackageReasonCode code) {
    switch (code) {
    case PKG_OK:
        break;
    case PKG_CORRUPTED:
        Error("Got corrupted package.\n");
        break;
    case PKG_FD_CLOSED:
        Error("Connection is closed by remote.\n");
        break;
    default:
        assert(0);
    }
}
