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

ReasonCode PackageReaderWriter::read(unsigned char* buff, size_t toRead) {
    while (toRead != 0) {
        ssize_t nbRead = ::read(_fd, buff, toRead);
        if (nbRead < 0) {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            case ECONNRESET:
                return RC_IO_CONN_RESET;
            default:
                perror(NULL);
                return RC_IO_UNKNOWN;
            }
        } else if (nbRead == 0) {
            return RC_IO_FD_CLOSED;
        } else {
            assert(toRead >= nbRead);
            toRead -= nbRead;
            buff += nbRead;
        }
    }
    return RC_OK;
}

ReasonCode PackageReaderWriter::readPackage(Package& package) {

    RawPackageHead head;
    ReasonCode code;

    CheckReasonCode(read((unsigned char*)&head, sizeof(head)));

    if (strncmp(head.magic, kPackageMagic, strlen(kPackageMagic))) {
        CheckReasonCode(RC_IO_PKG_CORRUPTED);
    } else if(head.size > kMaxPackageSize) {
        CheckReasonCode(RC_IO_PKG_CORRUPTED);
    } else {
        package.resize(head.size);
        CheckReasonCode(read(package.data(), head.size));
    }

onExit:
    return code;
}

ReasonCode PackageReaderWriter::write(const unsigned char* buff, size_t toWrite) {
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
            return RC_IO_FD_CLOSED;
        } else {
            assert(toWrite >= nbWrite);
            toWrite -= nbWrite;
            buff += nbWrite;
        }
    }
    return RC_OK;
}

ReasonCode PackageReaderWriter::writePackage(const Package& package) {
    RawPackageHead head;
    ReasonCode code;

    size_t size = package.size();
    memcpy(head.magic, kPackageMagic, strlen(kPackageMagic));
    memcpy(&head.size, &size, sizeof(size));

    CheckReasonCode(write((unsigned char*)&head, sizeof(head)));
    CheckReasonCode(write(package.data(), head.size));

onExit:
    return code;
}

void PackageReaderWriter::printError(ReasonCode code) {
    switch (code) {
    case RC_OK:
        break;
    case RC_IO_PKG_CORRUPTED:
        Error("Got corrupted package.\n");
        break;
    case RC_IO_FD_CLOSED:
        Error("Connection is closed by remote.\n");
        break;
    default:
        assert(0);
    }
}
