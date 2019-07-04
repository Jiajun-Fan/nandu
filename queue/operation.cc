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

ReasonCode Operation2String(const Operation& operation, std::string& str) {
    const Package* data = operation.getCdata();
    const unsigned char* buff = data->cData();
    size_t size = data->size();
    if (size == 0 || buff[size-1] != '\0') {
        return RC_OP_DATA_NOTSTRING;
    } else {
        str = std::string((const char*)buff);
        return RC_OK;
    }
}

ReasonCode String2Operation(const std::string& str, Operation& operation) {
    size_t size = str.length() + 1;
    Package* data = operation.getData();
    data->resize(size);
    memcpy(data->data(), str.c_str(), size);
    return RC_OK;
}
