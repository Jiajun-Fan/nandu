#include "reason_code.hh"
#include <assert.h>
#include <stdio.h>

const char* kErrorMessageTable[] = {
    NULL,
    "Can't connect to server.\n",
    "Got corrupted package.\n",
    "The file descriptor is already closed.\n",
    "Connection reset by peer.\n",
    "Unknown I/O error.\n",
    "The string used in Task is too long.\n",
    "Too many parameters in task.\n",
    "Unsupported opcode.\n",
    "Wrong opcode.\n",
    "Operation data is not a string.\n",
    "Double register operation handler.\n",
    "Server closed connection.\n",
};

void printError(ReasonCode code) {
    assert(code < (sizeof(kErrorMessageTable) / sizeof(const char*)));
    if (code != RC_OK) {
        Error(kErrorMessageTable[code]);
    }
}
