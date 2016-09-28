#include "tests.h"

int port = PORT;
char *host = "localhost";

int create_tcp_client() {
    int sock;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
        error("ERROR opening socket");

    int flag = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0) 
        error("setsockopt failed");

    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", host);
        exit(0);
    }

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(PORT);

    const struct sockadder *sock_conn = (struct sockadder*) &serveraddr;

    if (connect(sock, sock_conn, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");

    return sock;
}

long long test_latency(int buffer_size) {
    struct timespec t2, t3;
    char buf[buffer_size];
    int sock = create_tcp_client();

    // NOTE: keeping the second read in there seems to lower times. 
    // Possible the synchronization is helping here? Doublecheck with more iterations to be sure
    notzero(write(sock, &buffer_size, sizeof(int)), "control message failed");
    notzero(read(sock, &buffer_size, sizeof(int)), "read control message failed");

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    write(sock, buf, buffer_size);
    read(sock, buf, buffer_size);
    clock_gettime(CLOCK_MONOTONIC,  &t3);

    close(sock);
    return diff_time_ns(t2, t3) / 2;
}

// Same as latency without copying the data in its entirety
long long test_throughput(int buffer_size) {
    struct timespec t2, t3;
    char buf[buffer_size];
    int sock = create_tcp_client();

    notzero(write(sock, &buffer_size, sizeof(int)), "control message failed");
    notzero(read(sock, &buffer_size, sizeof(int)), "read control message failed");

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    write(sock, buf, buffer_size);
    read(sock, &buffer_size, sizeof(int));
    clock_gettime(CLOCK_MONOTONIC,  &t3);

    close(sock);
    return diff_time_ns(t2, t3);
}

void loop_latency(int iterations) {
    int sizes[] = {4, 16, 64, 256, 1*1024, 4*1024, 16*1024, 64*1024, 256*1024, 512*1024};
    long long min[10];
    int i, j;

    for (i= 0; i < 10; i++) {
        min[i] = DBL_MAX;
        for (j = 0; j < iterations; j++) {
            long long result = test_latency(sizes[i]);
            if (result < min[i]) 
                min[i] = result;
        }
    }

    for (i = 0; i < 10; i++) {
        printf("%lld, ", min[i]);
    }

    printf("\nLatency -- %d iterations\n", iterations);
}

void loop_throughput(int iterations) {
    int size = 1024 * 1024;
    int megs = 5;
    long long min = DBL_MAX;
    int j;

    for (j = 0; j < iterations; j++) {
        long long result = test_throughput(size * megs);

        if (result < min) 
            min = result;
    }

    printf("Throughput: %d MB in %d ns\n", megs, min);
}

void main(int argc, char ** argv) {
    int i = 1e4;
    loop_latency(i);
    // loop_throughput(i);
}