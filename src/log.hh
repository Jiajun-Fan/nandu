#pragma once
#include <stdarg.h>
#include <iostream>

typedef enum {
    kLogFatal = 0,
    kLogError,
    kLogWarning,
    kLogInfo,
    kLogNote,
    kLogDebug
} LogLevel;

extern LogLevel gLogLevel;
void SetLogLevel(LogLevel level);

inline void Fatal(const char* fmt, ...) {
    if (gLogLevel < kLogFatal) {
        return;
    }
    fprintf(stderr, "[ FATAL ]: ");
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
}

inline void Error(const char* fmt, ...) {
    if (gLogLevel < kLogError) {
        return;
    }
    fprintf(stderr, "[ ERROR ]: ");
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
}

inline void Warning(const char* fmt, ...) {
    if (gLogLevel < kLogWarning) {
        return;
    }
    fprintf(stderr, "[WARNING]: ");
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
}

inline void Info(const char* fmt, ...) {
    if (gLogLevel < kLogInfo) {
        return;
    }
    va_list argptr;
    fprintf(stderr, "[ INFO  ]: ");
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
}

inline void Note(const char* fmt, ...) {
    if (gLogLevel < kLogNote) {
        return;
    }
    fprintf(stderr, "[ Note  ]: ");
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
}

#ifndef NDEBUG
#define Debug(format, args...) \
    if (gLogLevel >= kLogDebug) { \
        fprintf(stderr, "[ DEBUG ]: (%s:%d) ", __FILE__, __LINE__); \
        fprintf(stderr, format, args); \
    }
#else
#define Debug(args...)
#endif
