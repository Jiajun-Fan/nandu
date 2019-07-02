#pragma once
typedef enum {
    RC_OK,
    RC_IO_PKG_CORRUPTED,
    RC_IO_FD_CLOSED,
    RC_TSK_EXEC_TOO_LONG,
    RC_TSK_TOOMANY_PARAMS,
    RC_TSK_BAD_CHAR,
} ReasonCode;
