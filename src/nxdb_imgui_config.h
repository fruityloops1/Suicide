#pragma once

#include "LogServer.h"

extern "C" {
#include "switch/runtime/diag.h"
}

inline void assertFail(const char* file, int line, const char* expr) {
    nxdb::log("%s:%d: Assert failed: %s", file, line, expr);
    diagAbortWithResult(0x602);
}

#define IM_ASSERT(_EXPR)                              \
    ([&]() {                                          \
        if (!(_EXPR))                                 \
            ::assertFail(__FILE__, __LINE__, #_EXPR); \
    })()
