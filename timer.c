#include "tests.h"

// Source: http://stackoverflow.com/questions/9048491/measuring-short-time-intervals-in-c-on-ubuntu-vmware
long long bench_timer() {
    struct timespec t2, t3;

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    clock_gettime(CLOCK_MONOTONIC,  &t3);

    return diff_time_ns(t2, t3);
}

long long time_syscall(){
    struct timespec t2, t3;
    pid_t tid;

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    tid = syscall(SYS_getpid);
    clock_gettime(CLOCK_MONOTONIC,  &t3);
    return diff_time_ns(t2, t3);
}

long long time_syscall2(){
    struct timespec t2, t3;
    pid_t tid;

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    tid = syscall(SYS_getuid);
    clock_gettime(CLOCK_MONOTONIC,  &t3);
    return diff_time_ns(t2, t3);
}

// void main(void) {
void main(void) {
    int iters = 1e6;
    long long timer, pid, uid, res;

    timer = DBL_MAX;
    pid = DBL_MAX;
    uid = DBL_MAX;

    for (int i = 0; i < iters; i++) {
        res = bench_timer();
        if (res < timer) 
            timer = res;
        
        res = time_syscall();
        if (res < pid) 
            pid = res;
        
        res = time_syscall2();
        if (res < uid) 
            uid = res;
    }

    
    printf("Timer: \t%lld\n", timer);
    printf("Syscall (pid): %lld\n", pid);
    printf("Syscall (uid): %lld\n", uid);
    printf("Iterations: %d\n", iters);
}


