#include "task.hh"
#include <string.h>
#include <sstream>
#include "exception.hh"
#include "log.hh"

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

void Package2Task(const Package& package, Task& task) {

    task.reset();

    std::string name;

    unsigned char* ptr = (unsigned char*)package.cData();
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
}

void Task2Package(const Task& task, Package& package) {

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
    package.resize(totalSize);

    ptr = package.data();
    writeString(ptr, ptr, task.getName());
    *(size_t*)(ptr) = task.getParams().size();
    ptr += sizeof(size_t);

    for (auto it = task.getParams().begin();
            it != task.getParams().end(); it++) {
        writeString(ptr, ptr, *it);
    }
}
