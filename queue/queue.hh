#pragma once
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <list>
#include <iostream>
#include <pthread.h>

class Param {
public:
    virtual char getType() const = 0;
    virtual uint32_t getSize() const = 0;
    virtual const void* getBuffer() const = 0;
    virtual ~Param() {}
};

class IntParam : public Param {
public:
    IntParam(int64_t v) : _v(v) {}
    virtual ~IntParam() {}
    virtual char getType() const { return 'c'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    int64_t                                         _v;
};

class StringParam : public Param {
public:
    StringParam(const std::string& v) : _v(v) {}
    virtual ~StringParam() {}
    virtual char getType() const { return 's'; }
    virtual uint32_t getSize() const { (uint32_t)_v.length(); }
    virtual const void* getBuffer() const { return _v.c_str(); };
private:
    std::string                                     _v;
};

class DoubleParam : public Param {
public:
    DoubleParam(double v) : _v(v) {}
    virtual ~DoubleParam() {}
    virtual char getType() const { return 'd'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    double                                          _v;
};

class BooleanParam : public Param {
public:
    BooleanParam(bool v) : _v(v) {}
    virtual ~BooleanParam() {}
    virtual char getType() const { return 'b'; }
    virtual uint32_t getSize() const { return sizeof(_v); };
    virtual const void* getBuffer() const { return &_v; };
private:
    bool                                            _v;
};

class Task {
public:
    Task(const std::string& name) : _name(name) {}
    virtual ~Task() {}
    void addParam(Param* param) {
        _params.push_back(std::unique_ptr<Param>(param));
    }
private:
    std::string                                     _name;
    std::list<std::unique_ptr<Param> >              _params;
};

class Queue {
public:
    Queue() : _id(0) {}
    ~Queue() {}

    void addTask(uint32_t ts, Task* task);

private:
    std::map<uint64_t, std::unique_ptr<Task> >      _tasks;
    uint32_t                                        _id;
};

