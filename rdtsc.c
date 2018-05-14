
#include "rdtsc.h"

#include <time.h> // for timespec
#ifdef __MACH__
#include <mach/mach_time.h> // for clock_serv_t
#include <mach/clock.h>
#include <mach/mach.h>
#endif

/* 这个计时是获得时钟周期 比time 更贴切*/
#ifndef WIN32
uint64_t rdtsc2()
{
    unsigned long long int x;
    asm volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
uint64_t rdtsc3()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
uint64_t rdtsc4()
{
    union{
        uint64_t tsc_64;
        struct {
            uint32_t lo_32;
            uint32_t hi_32;
        };
    }tsc;
    asm volatile ("rdpmc": "=a" (tsc.lo_32), "=d" (tsc.hi_32));
    return tsc.tsc_64;
}
#endif

uint64_t rdtsc1()
{

#ifdef WIN32
    return __rdtsc();
#else
    struct timespec ts;
    uint64_t tsc;
#ifdef  __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
#else
    while(0 !=clock_gettime(CLOCK_MONOTONIC, &ts))
        ;
    // and CLOCK_MONOTONIC_RAW ?
#endif
    tsc = (uint64_t)ts.tv_sec;
    tsc *= 1000000000;
    tsc += ts.tv_nsec;
    return tsc; // 这个数量级与asm 计算的一致 但是偏小 
#endif
}

uint64_t rdtsc()
{
#ifdef WIN32
    return __rdtsc();
#else
    unsigned long long int x;
    asm volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
#endif
}
