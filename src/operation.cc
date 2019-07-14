#include "operation.hh"
#include <string.h>

void Operation::read(int fd) {
    PackageReaderWriter(fd).readPackage(*this);
}

void Operation::write(int fd) const {
    PackageReaderWriter(fd).writePackage(*this);
}

const unsigned char* OperationData::cData() const {
    return _op->cData() + sizeof(int);
}
unsigned char* OperationData::data() { 
    return _op->data() + sizeof(int);
}
void OperationData::resize(size_t size) {
    _op->resize(size + sizeof(int));
}
size_t OperationData::size() const { 
    size_t rawSize = _op->size();
    assert(rawSize >= sizeof(int));
    return rawSize - sizeof(int);
}

