#pragma once
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "io.hh"

enum {
    SVC_SYS = 0,
    SVC_AUTH, 
    SVC_TASK,
};

enum {
    OP_BAD = -1,
    OP_DONE,
    OP_INIT,
};

inline constexpr int genOperationCode(int svc, int subOp) { return (svc << 8) + subOp; }
inline constexpr int genSvcCode(int opCode) { return opCode >> 8; }

class Operation;
class OperationData : public Package {
public:
    explicit OperationData(Operation* op) : _op(op) {}

    // Package interface
    virtual const unsigned char* cData() const;
    virtual unsigned char* data();
    virtual void resize(size_t size);
    virtual size_t size() const;
private:
    Operation*                      _op;
};

class Operation : public Package {
public:
    explicit Operation(int opCode) : _dataSize(0), _opData(this) {
        _raw = (int*)malloc(sizeof(int));
        *_raw = opCode;
    }
    Operation(int svcCode, int subOp) : _dataSize(0), _opData(this) {
        _raw = (int*)malloc(sizeof(int));
        *_raw = genOperationCode(svcCode, subOp);
    }
    Operation() : _dataSize(0), _opData(this) {
        _raw = (int*)malloc(sizeof(int));
        *_raw = OP_BAD;
    }
    virtual ~Operation() {
        // it's OK to free NULL, no check needed
        free(_raw);
    }
    void read(int fd);
    void write(int fd) const;

    int getOpCode() const { return *_raw; }
    void setOpCode(int op) { *_raw = op; }

    size_t getDataSize() const { return _dataSize; }

    const Package& getCData() const { return _opData; }
    Package& getData() { return _opData; }

    // Package interface
    virtual const unsigned char* cData() const { return (const unsigned char*)_raw; }
    virtual unsigned char* data() { return (unsigned char*)_raw; }
    virtual void resize(size_t size) {
        assert(size >= sizeof(int));
        _raw = (int*)realloc(_raw, size);
        _dataSize = size - sizeof(int);
    }
    virtual size_t size() const { return sizeof(int) + _dataSize; }
    virtual std::string toString() const { return getCData().toString(); }
    virtual void fromString(const std::string& str) { getData().fromString(str); }
    virtual Task toTask() const { return getCData().toTask(); }
    virtual void fromTask(const Task& task) { getData().fromTask(task); }

    Operation& operator=(const Operation& old) {
        _dataSize = old.getDataSize();
        _raw = (int*)malloc(old.size());
        memmove(_raw, old.cData(), old.size());
    }

private:
    int*                        _raw;
    size_t                      _dataSize;
    OperationData               _opData;
};

