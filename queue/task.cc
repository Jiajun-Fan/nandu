#include "task.hh"
#include <string.h>
#include "log.hh"

static ReasonCode readString(unsigned char* buff, size_t& ptr, std::string& str) {
    size_t size = *(size_t*)buff;
    if (size > kMaxTaskLength) {
        Error("Parameter too long.\n");
        return RC_TSK_EXEC_TOO_LONG;
    }
    char cStr[kMaxTaskLength+1];
    memcpy(cStr, buff+sizeof(size), size);
    cStr[size] = '\0';
    str = std::string(cStr);
    ptr = size + sizeof(size);
    return RC_OK;
}

static ReasonCode writeString(unsigned char* buff, size_t& ptr, std::string str) {
    size_t size = str.length();
    if (size > kMaxTaskLength) {
        Error("Parameter too long.\n");
        return RC_TSK_EXEC_TOO_LONG;
    }
    memcpy(buff, &size, sizeof(size));
    memcpy(buff+sizeof(size), str.c_str(), size);
    ptr = size + sizeof(size);
    return RC_OK;
}

ReasonCode CreateTaskFromPackage(Package& package, Task& task) {
    std::string name;
    size_t offset = 0;
    ReasonCode code = readString(package.data(), offset, name);
    if (code != RC_OK) {
        return code;
    }

    task.setName(name);
    size_t nbParam = *(size_t*)(package.data()+offset);
    if (nbParam > kMaxParamNum) {
        Error("Too many parameters.\n");
        return RC_TSK_TOOMANY_PARAMS;
    }
    offset += sizeof(size_t);

    for (unsigned i = 0; i < nbParam; i++) {
        size_t len = 0;
        std::string param;
        ReasonCode code = readString(package.data()+offset, len, param);
        if (code != RC_OK) {
            return code;
        }
        task.addParam(param);
        offset += len;
    }

    return RC_OK;
}

ReasonCode Task::package(Package& package) {
    if (_params.size() > kMaxParamNum) {
        Error("Too many parameters.\n");
        return RC_TSK_TOOMANY_PARAMS;
    }
    // size of name
    size_t totalSize = _name.length() + sizeof(size_t);
    // size of number of parameters
    totalSize += sizeof(size_t);
    for (auto it = _params.begin(); it != _params.end(); it++) {
        totalSize += (it->length() + sizeof(size_t));
    }
    package.resize(totalSize);

    size_t offset = 0;
    ReasonCode code = writeString(package.data(), offset, _name);
    *(size_t*)(package.data()+offset) = _params.size();
    offset += sizeof(size_t);

    for (auto it = _params.begin(); it != _params.end(); it++) {
        size_t len = 0;
        ReasonCode code = writeString(package.data()+offset, len, *it);
        if (code != RC_OK) {
            return code;
        }
        offset += len;
    }
    return RC_OK;
}
