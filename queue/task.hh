#pragma once
#include <stdint.h>
#include <limits.h>
#include <memory>
#include <string>
#include <list>
#include "io.hh"

#define kMaxTaskLength PATH_MAX
#define kMaxParamNum PATH_MAX

typedef enum {
    TSK_OK = 0,
    TSK_TOO_LONG,
    TSK_TOOMANY_PARAMS,
    TSK_BAD_CHAR,
} TaskReasonCode;

class Task;

extern TaskReasonCode CreateTaskFromPackage(Package& package, Task& task);
extern TaskReasonCode CreatePackageFromTask(Package& package, Task& task);

#ifdef ParamTypes

class Param {
public:
    virtual char getType() const = 0;
    virtual uint32_t getSize() const = 0;
    virtual const void* getBuffer() const = 0;
    virtual ~Param() {}
};

class IntParam : public Param {
public:
    explicit IntParam(int64_t v) : _v(v) {}
    virtual ~IntParam() {}
    virtual char getType() const { return 'c'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    int64_t                                         _v;
};

class StringParam : public Param {
public:
    explicit StringParam(const std::string& v) : _v(v) {}
    virtual ~StringParam() {}
    virtual char getType() const { return 's'; }
    virtual uint32_t getSize() const { (uint32_t)_v.length(); }
    virtual const void* getBuffer() const { return _v.c_str(); };
private:
    std::string                                     _v;
};

class DoubleParam : public Param {
public:
    explicit DoubleParam(double v) : _v(v) {}
    virtual ~DoubleParam() {}
    virtual char getType() const { return 'd'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    double                                          _v;
};

class BooleanParam : public Param {
public:
    explicit BooleanParam(bool v) : _v(v) {}
    virtual ~BooleanParam() {}
    virtual char getType() const { return 'b'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    bool                                            _v;
};

class Task {
public:
    Task() : _name("") {}
    explicit Task(const std::string& name) : _name(name) {}
    virtual ~Task() {}
    void addParam(Param* param) {
        _params.push_back(std::unique_ptr<Param>(param));
    }
    const std::string& getName() const { return _name; }
    void setName(const std::string& name) { _name = name; }
private:
    std::string                                     _name;
    std::list<std::unique_ptr<Param> >              _params;
};

#else

// currently we are not sure if parameter type other than 
// string is needed. So keep the old code
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
    TaskReasonCode package(Package& package);
private:
    std::string                                     _name;
    std::list<std::string>                          _params;
};
#endif
