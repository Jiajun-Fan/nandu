#include "reason_code.hh"
#include <assert.h>
#include <stdio.h>

const char* kErrorMessageTable[] = {
    NULL,
    "Got corrupted package.\n",
    "The file descriptor is already closed.\n",
    "Connection reset by peer.\n",
    "Unknown I/O error.\n",
    "The string used in Task is too long.\n",
    "Too many parameters in task.\n",
    "Wrong opcode.\n",
    "Bad hash.\n"
};

void printError(ReasonCode code) {
    assert(code < (sizeof(kErrorMessageTable) / sizeof(const char*)));
    if (code != RC_OK) {
        Error(kErrorMessageTable[code]);
    }
}
