#include "tests.h"

long long udp_lat(int buffer_size) {
    struct sockaddr_in serveraddr;
    struct hostent *server;
    int serverlen;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    notzero(sock, "socket creation failed");

    server = gethostbyname(HOST);
    if (server == NULL) 
        error("gethostbyname failed");

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(PORT);
    serverlen = sizeof(serveraddr);

    // Client Code
    struct timespec t2, t3;
    char buf[buffer_size];
    bzero(buf, buffer_size);

    notzero(sendto(sock, &buffer_size, sizeof(int), 0, &serveraddr, serverlen), "control message failed");
    notzero(recvfrom(sock, &buffer_size, sizeof(int), 0, &serveraddr, &serverlen), "read control message failed");

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    sendto(sock, &buf, buffer_size, 0, &serveraddr, serverlen);
    recvfrom(sock, &buf, buffer_size, 0, &serveraddr, &serverlen);
    clock_gettime(CLOCK_MONOTONIC,  &t3);

    close(sock);
    return diff_time_ns(t2, t3) / 2;
}

long long udp_through(int buffer_size, int mtu) {
    struct timespec t2, t3;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    int serverlen, written, slice_index, i, n, optval;

    int num_packets = buffer_size / mtu;
    char buf[mtu];
    bzero(buf, mtu);
    buf[0] = 'a';

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    notzero(sock, "socket creation failed");

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;//10000; // ns 100

    optval = 1;
    notzero(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)), "setsockopt faild");
    notzero(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)), "setsockopt faild");

    // The other option is to have a control socket just for losses which gets ackd
    // Could also just use TCP here, but have to show the difference is negligible

    server = gethostbyname(HOST);
    if (server == NULL) 
        error("gethostbyname failed");

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(PORT);
    serverlen = sizeof(serveraddr);

    // Client Code
    notzero(sendto(sock, &buffer_size, sizeof(int), 0, &serveraddr, serverlen), "control message failed");
    notzero(sendto(sock, &num_packets, sizeof(int), 0, &serveraddr, serverlen), "control message failed");
    notzero(recvfrom(sock, &buffer_size, sizeof(int), 0, &serveraddr, &serverlen), "read ack control message failed");

    clock_gettime(CLOCK_MONOTONIC,  &t2);

    for (i = 0; i < num_packets; i++) {
        n = sendto(sock, &buf, mtu, 0, &serveraddr, serverlen);

        if (n <= 0) {
            printf("Lost a packet: %d\n", n);
        }
    }

    // Send a kill command if the peer hasn't acked
    if (recvfrom(sock, &buffer_size, sizeof(int), 0, &serveraddr, &serverlen) < 0) {
        buf[0] = '\0';
        printf("Lost packets. Sending kill\n");
        sendto(sock, &buf, mtu, 0, &serveraddr, serverlen);
        recvfrom(sock, &buffer_size, sizeof(int), 0, &serveraddr, &serverlen);
    }


    clock_gettime(CLOCK_MONOTONIC,  &t3);
    close(sock);
    return diff_time_ns(t2, t3) / 2;
}

void loop_latency(int iterations) {
    // Note: testing with packet sizes greater than 4096 did not see the full message get through.
    int sizes[] = {4, 16, 64, 256, 1*1024, 4*1024};
    long long min[10];
    int i, j;

    // For each message size * number of iterations
    for (i= 0; i < 6; i++) {
        int size = sizes[i];
        min[i] = DBL_MAX;

        for (j = 0; j < iterations; j++) {
            long long result = udp_lat(size);

            if (result < min[i]) 
                min[i] = result;
        }
    }

    for (i = 0; i < 6; i++) {
        printf("%lld, ", min[i]);
    }

    printf("\nLatency -- %d iterations\n", iterations);
}

void loop_throughput(int iterations, int kb) {
    // Note: testing with packet sizes greater than 4096 did not see the full message get through.
    int data_size = 1024 * 1024 * kb;
    int sizes[] = {4, 16, 64, 256, 1*1024, 4*1024};
    int sizes_len = 6;
    long long min[sizes_len];
    int i, j;
    
    for (i= 0; i < sizes_len; i++) {
        int size = sizes[i];
        min[i] = DBL_MAX;

        for (j = 0; j < iterations; j++) {
            long long result = udp_through(data_size, size);
            if (result < min[i]) 
                min[i] = result;
        }
    }

    for (i = 0; i < sizes_len; i++) {
        printf("%lld, ", min[i]);
    }

    printf("\nThroughput -- %d iterations\n", iterations);
}


void main(int argc, char **argv) {
    printf("Client started\n");
    // loop_latency(1e5);
    loop_throughput(100, 1);
}