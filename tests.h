#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <float.h>

#define PORT 8888
#define HOST "127.0.0.1"
#define ITER 1


// Initial server implementations taken from https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/udpserver.c
void error(char *msg) {
    perror(msg);
    exit(0);
}

// Print an error and exit if the given int is less than 0
void notzero(int op, char *message) {
    if (op < 0) {
        perror(message);
        exit(1);
    }
}

// Simple little control struct. Used to have more in it, worth a refactor
struct control {
    int byte_size;
};

// send all and recv all implementations. Fails immediately if an error occurs
// Works except that I haven't seen segmentation problems
// Usage: write_all(sock, &buf, buffer_size);
void write_all(int sock, char *buf[], size_t len) {
    int written = 0;
    int n; 

    while (written < len) {
        n = write(sock, &buf[written], len - written);
        
        if (n < 0) 
            error("failed in send_all");

        written += n;
        // printf("Wrote %d of %d bytes\n", written, len);
    }
}

// Return the difference between two timespecs in nanoseconds. Expects the second time to be later
long long diff_time_ns(struct timespec t1, struct timespec t2) {
    return ((long long)t2.tv_sec * 1.0e9 + t2.tv_nsec) - 
           ((long long)t1.tv_sec * 1.0e9 + t1.tv_nsec);
}

#endif