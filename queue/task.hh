#pragma once
#include <stdint.h>
#include <limits.h>
#include <string>
#include <list>
#include "io.hh"
#include "reason_code.hh"

#define kTaskMaxStringLength PATH_MAX
#define kTaskMaxParamNum PATH_MAX

class Task;

extern ReasonCode CreateTaskFromPackage(Package& package, Task& task);

class Task {
public:
    Task() : _name("") {}
    explicit Task(const std::string& name) : _name(name) {}
    virtual ~Task() {}
    void addParam(const std::string& param) {
        _params.push_back(param);
    }
    const std::string& getName() const { return _name; }
    void setName(const std::string& name) { _name = name; }
    ReasonCode package(Package& package);
    void printTask();
    void reset() { _name = ""; _params.clear(); }
private:
    std::string                                     _name;
    std::list<std::string>                          _params;
};
