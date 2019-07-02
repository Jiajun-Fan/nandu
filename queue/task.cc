#include "task.hh"
#include <string.h>
#include <sstream>
#include "log.hh"

static inline ssize_t getStringLength(const unsigned char* buff) {
    for (ssize_t i = 0; i < kTaskMaxStringLength; i++) {
        if (buff[i] == 0) {
            return i+1;
        }
    }
    return -1;
}

static ReasonCode readString(const unsigned char* buff, unsigned char*& ptr, std::string& str) {
    ssize_t strLength = getStringLength(buff);
    if (strLength < 0) {
        return RC_TSK_STRING_TOO_LONG;
    }
    ptr = (unsigned char*)buff + strLength;
    str = std::string((const char*)buff);

    return RC_OK;
}

static ReasonCode writeString(unsigned char* buff, unsigned char*& ptr, std::string str) {
    size_t strLength = str.length()+1;
    if (strLength > kTaskMaxStringLength) {
        return RC_TSK_STRING_TOO_LONG;
    }
    memcpy(buff, str.c_str(), strLength);
    ptr = (unsigned char*)buff + strLength;
    return RC_OK;
}

ReasonCode CreateTaskFromPackage(Package& package, Task& task) {

    task.reset();

    std::string name;

    unsigned char* ptr = package.data();
    ReasonCode code;
    size_t nbParam = 0;
    CheckReasonCode(readString(ptr, ptr, name));
    task.setName(name);

    nbParam = *(size_t*)(ptr);
    if (nbParam > kTaskMaxParamNum) {
        CheckReasonCode(RC_TSK_TOOMANY_PARAMS);
    }
    ptr += sizeof(size_t);

    for (unsigned i = 0; i < nbParam; i++) {
        std::string param;
        CheckReasonCode(readString(ptr, ptr, param));
        task.addParam(param);
    }

onExit:
    return code;
}

ReasonCode Task::package(Package& package) {
    ReasonCode code;

    // size of name
    size_t totalSize = _name.length() + 1;
    unsigned char* ptr = NULL;

    // number of parameters 
    if (_params.size() > kTaskMaxParamNum) {
        CheckReasonCode(RC_TSK_TOOMANY_PARAMS);
    }
    totalSize += sizeof(size_t);

    for (auto it = _params.begin(); it != _params.end(); it++) {
        totalSize += (it->length() + 1);
    }
    package.resize(totalSize);

    ptr = package.data();
    CheckReasonCode(writeString(ptr, ptr, _name));
    *(size_t*)(ptr) = _params.size();
    ptr += sizeof(size_t);

    for (auto it = _params.begin(); it != _params.end(); it++) {
        CheckReasonCode(writeString(ptr, ptr, *it));
    }

onExit:
    return code;
}

void Task::printTask() {
    std::stringstream ss;
    ss << _name << " ";
    for (auto it = _params.begin(); it != _params.end(); it++) {
        ss << *it << " ";
    }
    Info("%s\n", ss.str().c_str());
}
