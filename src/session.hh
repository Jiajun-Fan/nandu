#pragma once
#include <string>
#include <stdint.h>

class Service;
enum {
    SC_BAD = -1,
    SC_DONE,
    SC_INIT,
};

inline constexpr int genStateCode(int svc, int subSc) { return (svc << 8) + subSc; }

typedef struct {
    int                         fd;
    int                         curState;
    // use Package to replace string if needed
    std::string                 data;
} Session;
