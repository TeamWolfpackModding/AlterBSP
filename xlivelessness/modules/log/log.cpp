#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include "../../thirdparty/easyloggingpp/easyloggingpp.h"
#include "../../thirdparty/subhook/subhook.h"
#include "../../global.h"
#include "log.h"

subhook_t log_hook;
subhook_flags_t log_hook_flags;

DECLSPEC_NOINLINE int log_detour(const char* format, ...) 
{
    va_list args;
    va_start(args, format);

    int length = vsnprintf(nullptr, 0, format, args);
    assert(length >= 0);

    char* buf = new char[length + 1];
    vsnprintf(buf, length + 1, format, args);
    
    CLOG(INFO, "engine") << buf;

    delete[] buf;
    va_end(args);

    return 0;
}

void InitLog() 
{
    log_hook = subhook_new((void*)(baseAddress + 0x254B0), (void*)log_detour, log_hook_flags);
    subhook_install(log_hook);
}