#include "reason_code.hh"
#include <stdio.h>

void printError(ReasonCode code) {

    switch (code) {
    case RC_OK:
        break;
    case RC_IO_PKG_CORRUPTED:
        Error("Got corrupted package.\n");
        break;
    case RC_IO_FD_CLOSED:
        Error("The file descriptor is already closed.\n");
        break;
    case RC_TSK_STRING_TOO_LONG:
        Error("The string used in Task is too long.\n");
        break;
    case RC_TSK_TOOMANY_PARAMS:
        Error("Too many parameters in task.\n");
        break;
    case RC_ND_WRONG_CODE:
        Error("Got wrong opcode.\n");
        break;
    case RC_ND_BAD_HASH:
        Error("Got bad hash.\n");
        break;
    default:
        break;
    }

}
