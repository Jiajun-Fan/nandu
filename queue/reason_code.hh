#pragma once
#include "log.hh"
typedef enum {
    RC_OK,
    RC_IO_PKG_CORRUPTED,
    RC_IO_FD_CLOSED,
    RC_IO_CONN_RESET,
    RC_IO_UNKNOWN,
    RC_TSK_STRING_TOO_LONG,
    RC_TSK_TOOMANY_PARAMS,
    RC_ND_WRONG_CODE,
    RC_ND_BAD_HASH,
} ReasonCode;

#define CheckReasonCode(p) code = p; \
    if (code != RC_OK) { \
        goto onExit; \
    }

extern void printError(ReasonCode);
