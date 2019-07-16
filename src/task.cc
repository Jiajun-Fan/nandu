#include "task.hh"
#include <stdlib.h>
#include <sstream>

// need to check the return value in the future
void Task::run() {
    std::stringstream ss;
    ss << _name;
    for (auto it = _params.begin(); it != _params.end(); it++) {
        ss << " " <<  (*it);
    }
    system(ss.str().c_str());
}
