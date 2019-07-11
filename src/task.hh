#pragma once
#include <stdint.h>
#include <limits.h>
#include <string>
#include <list>
#include "io.hh"

#define kTaskMaxStringLength PATH_MAX
#define kTaskMaxParamNum PATH_MAX

class Task;

class Task {
public:
    Task() : _name("") {}
    explicit Task(const std::string& name) : _name(name) {}
    virtual ~Task() {}
    void addParam(const std::string& param) {
        _params.push_back(param);
    }
    const std::list<std::string>& getParams() const { return _params; }
    const std::string& getName() const { return _name; }
    void setName(const std::string& name) { _name = name; }

    void reset() { _name = ""; _params.clear(); }
private:
    std::string                                     _name;
    std::list<std::string>                          _params;
};
