#include "package.hh"
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

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

std::unique_ptr<Package> PackageReaderWriter::readPackage() {
    std::unique_ptr<Package> ret = std::unique_ptr<Package>(new Package);
    RawPackageHead head;

    ret->code = read((unsigned char*)&head, sizeof(head));
    if (strncmp(head.magic, kPackageMagic, strlen(kPackageMagic))) {
        ret->code = PKG_CORRUPTED;
    } else if(head.size > kMaxPackageSize) {
        ret->code = PKG_CORRUPTED;
    } else {
        ret->bytes.resize(head.size);
        ret->code = read(ret->bytes.data(), head.size);
    }
    return ret;
}
