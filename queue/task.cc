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

ReasonCode Package2Task(const Package& package, Task& task) {

    task.reset();

    std::string name;

    unsigned char* ptr = (unsigned char*)package.cData();
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

ReasonCode Task2Package(const Task& task, Package& package) {
    ReasonCode code;

    // size of name
    size_t totalSize = task.getName().length() + 1;
    unsigned char* ptr = NULL;

    // number of parameters 
    if (task.getParams().size() > kTaskMaxParamNum) {
        CheckReasonCode(RC_TSK_TOOMANY_PARAMS);
    }
    totalSize += sizeof(size_t);

    for (auto it = task.getParams().begin();
            it != task.getParams().end(); it++) {
        totalSize += (it->length() + 1);
    }
    package.resize(totalSize);

    ptr = package.data();
    CheckReasonCode(writeString(ptr, ptr, task.getName()));
    *(size_t*)(ptr) = task.getParams().size();
    ptr += sizeof(size_t);

    for (auto it = task.getParams().begin();
            it != task.getParams().end(); it++) {
        CheckReasonCode(writeString(ptr, ptr, *it));
    }

onExit:
    return code;
}
