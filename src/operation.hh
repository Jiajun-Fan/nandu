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

class OperationCode {
public:
    OperationCode() : _opCode(OP_BAD) {}
    explicit OperationCode(int opCode) : _opCode(opCode) {}
    OperationCode(int svc, int subOp) :
                    _opCode(genOpCode(svc, subOp)) {}
    virtual ~OperationCode() {}

    int getSvcCode() const { return _opCode >> 8; }
    int getOpCode() const { return _opCode; }

    int genOpCode(int svc, int subOp) { return (svc << 8) + subOp; }
private:
    int                     _opCode;
};

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
        _raw = (OperationCode*)malloc(sizeof(OperationCode));
        *_raw = OperationCode(opCode);
    }
    Operation(int svcCode, int subOp) : _dataSize(0), _opData(this) {
        _raw = (OperationCode*)malloc(sizeof(OperationCode));
        *_raw = OperationCode(svcCode, subOp);
    }
    explicit Operation(const OperationCode& code) : _dataSize(0), _opData(this) {
        _raw = (OperationCode*)malloc(sizeof(OperationCode));
        *_raw = code;
    }
    Operation() : _dataSize(0), _opData(this) {
        _raw = (OperationCode*)malloc(sizeof(OperationCode));
        *_raw = OperationCode();
    }
    virtual ~Operation() {
        free(_raw);
    }
    void read(int fd);
    void write(int fd) const;

    int getOpCode() const { return _raw->getOpCode(); }
    void setOpCode(int op) { *_raw = OperationCode(op); }

    const Package& getCData() const { return _opData; }
    Package& getData() { return _opData; }

    // Package interface
    virtual const unsigned char* cData() const { return (const unsigned char*)_raw; }
    virtual unsigned char* data() { return (unsigned char*)_raw; }
    virtual void resize(size_t size) {
        assert(size >= sizeof(OperationCode));
        _raw = (OperationCode*)realloc(_raw, size);
        _dataSize = size - sizeof(OperationCode);
    }
    virtual size_t size() const { return sizeof(OperationCode) + _dataSize; }
    virtual std::string toString() const { return getCData().toString(); }
    virtual void fromString(const std::string& str) { getData().fromString(str); }
    virtual Task toTask() const { return getCData().toTask(); }
    virtual void fromTask(const Task& task) { getData().fromTask(task); }

    /*Operation& operator=(const Operation& op) {
        _raw = (OperationCode*)malloc(op.size());
        _dataSize = op.size() - sizeof(OperationCode);
        memcpy(_raw, op.cData(), op.size());
    }*/

private:
    OperationCode*              _raw;
    size_t                      _dataSize;
    OperationData               _opData;
};

