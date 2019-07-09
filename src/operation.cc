#include "operation.hh"
#include <string.h>

ReasonCode Operation::read(int fd) {
    return PackageReaderWriter(fd).readPackage(*this);
}

ReasonCode Operation::write(int fd) {
    return PackageReaderWriter(fd).writePackage(*this);
}

const unsigned char* OperationData::cData() const {
    return _op->cData() + sizeof(OperationCode);
}
unsigned char* OperationData::data() { 
    return _op->data() + sizeof(OperationCode);
}
void OperationData::resize(size_t size) {
    _op->resize(size + sizeof(OperationCode));
}
size_t OperationData::size() const { 
    size_t rawSize = _op->size();
    assert(rawSize >= sizeof(OperationCode));
    return rawSize - sizeof(OperationCode);
}

