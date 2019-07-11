#include <assert.h>
#include <exception>

typedef enum {
    RC_OK = 0,
    RC_IO_CANNOT_CONNECT,
    RC_IO_PKG_CORRUPTED,
    RC_IO_FD_CLOSED,
    RC_IO_CONN_RESET,
    RC_IO_UNKNOWN,
    RC_TSK_STRING_TOO_LONG,
    RC_TSK_TOOMANY_PARAMS,
    RC_OP_UNSUPPORTED,
    RC_OP_WRONG_CODE,
    RC_OP_DATA_NOTSTRING,
    RC_SVC_DUP_OPCODE,
    RC_SERVER_CLOSED,
    RC_BAD_REASON_CODE
} ReasonCode;

class Exception : public std::exception {
public:
    virtual const char* what() const throw() {
        return _msgTable[_code];
    }
    explicit Exception(ReasonCode code) : _code(code) {
        assert(code > RC_OK && code < RC_BAD_REASON_CODE);
    }
    virtual ~Exception() {}
private:
    static const char*                  _msgTable[];
    ReasonCode                          _code;
};
