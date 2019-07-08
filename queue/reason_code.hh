#pragma once
#include "log.hh"
typedef enum {
    RC_OK,
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
} ReasonCode;

#define CheckReasonCode(p) code = p; \
    if (code != RC_OK) { \
        goto onExit; \
    }

extern void printError(ReasonCode);
