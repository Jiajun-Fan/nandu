#include "log.hh"

LogLevel gLogLevel = kLogWarning;

void SetLogLevel(LogLevel level) {
    gLogLevel = level;
}
