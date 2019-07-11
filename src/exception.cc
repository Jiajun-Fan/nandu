#include "exception.hh"
#include <stdio.h>

const char* Exception::_msgTable[] = {
    NULL,
    "Can't connect to server.",
    "Got corrupted package.",
    "The file descriptor is already closed.",
    "Connection reset by peer.",
    "Unknown I/O error.",
    "The string used in Task is too long.",
    "Too many parameters in task.",
    "Unsupported opcode.",
    "Wrong opcode.",
    "Operation data is not a string.",
    "Double register operation handler.",
    "Server closed connection.",
};
