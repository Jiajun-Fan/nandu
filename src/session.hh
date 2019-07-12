#pragma once
#include <string>
#include <stdint.h>

class Service;
enum {
    SC_DONE,
    SC_AUTH_INIT,
    SC_INIT,
    SC_BAD,
};

typedef struct {
    int                         fd;
    int                         curState;
    // use Package to replace string if needed
    std::string                 data;
} Session;
