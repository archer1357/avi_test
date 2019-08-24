#include "timer.h"

#include <stdint.h>

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/time.h>
#endif


uint64_t getRawTime() {
#if defined(_MSC_VER)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (((uint64_t)ft.dwHighDateTime<<32)|ft.dwLowDateTime)/10;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (uint64_t)tv.tv_sec*(uint64_t)1000000+(uint64_t)tv.tv_usec;
#endif
}

double timer() {
    uint64_t cur=getRawTime();
    static uint64_t start=0;
    if(!start) { start=cur; }
    return (double)(cur-start)/1000000.0;
}
