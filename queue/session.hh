#pragma once
#include <string>

class Service;
typedef enum {
    S_INIT,
    S_DONE,
    S_AUTH_INIT,
    S_AUTH_WAIT_HASH, 
    C_INIT,
    C_DONE,
    C_AUTH_INIT,
    C_AUTH_WAIT_RESULT,
    C_TASK_PUSH,
    C_TASK_POP,
} StateCode;

typedef struct {
    int                         fd;
    StateCode                   curState;
    // use Package to replace string if needed
    std::string                 data;
    bool                        send;
} Session;
