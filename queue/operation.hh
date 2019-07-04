#pragma once
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "io.hh"

typedef enum {
    OP_DONE = 0,
    OP_AUTH_INIT, 
    OP_AUTH_HASH,
    OP_AUTH_OK,
    OP_PUSH,
    OP_POP,
    OP_BAD_OPERATION
} OperationCode;

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
    Operation() : _dataSize(0), _opData(this) {
        _raw = (OperationCode*)malloc(sizeof(OperationCode));
        *_raw = OP_BAD_OPERATION;
    }
    virtual ~Operation() {
        free(_raw);
    }
    ReasonCode read(int fd);
    ReasonCode write(int fd);

    OperationCode opCode() const { return *_raw; }
    void setOpCode(OperationCode op) { *_raw = op; }

    const Package* getCdata() const { return &_opData; }
    Package* getData() { return &_opData; }

    // Package interface
    const unsigned char* cData() const { return (const unsigned char*)_raw; }
    unsigned char* data() { return (unsigned char*)_raw; }
    void resize(size_t size) {
        assert(size >= sizeof(OperationCode));
        _raw = (OperationCode*)realloc(_raw, size);
        _dataSize = size - sizeof(OperationCode);
    }
    size_t size() const { return sizeof(OperationCode) + _dataSize; }

private:
    OperationCode*              _raw;
    size_t                      _dataSize;
    OperationData               _opData;
};

extern ReasonCode Operation2String(const Operation& operation, std::string& str);
extern ReasonCode String2Operation(const std::string& str, Operation& operation);
