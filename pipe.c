#include "tests.h"

// Child sends, parent receives. Since pipes are local to the machine we can just use clock 
// values as reported by the system on either end of the pipe. 
// Every test cycle consists of three writes: the size of the incoming write, the write, and finally
// the system time at the child immediately before the send. 
long long parent(int data_fd[], int control_fd[]) {
    int n, c;
    struct timespec t2, t3;

    notzero(write(control_fd[1], &c, sizeof(c)), "control write failed");
    notzero(read(data_fd[0], &n, sizeof(n)), "unable to read size control message");
    char buf[n];

    notzero(read(data_fd[0], &buf, n), "unable to read size control message");
    clock_gettime(CLOCK_MONOTONIC,  &t3);
    notzero(read(data_fd[0], &t2, sizeof(struct timespec)), "unable to read time control message");

    return diff_time_ns(t2, t3) / 2;
}

void child(int data_fd[], int control_fd[], int buffer_size) {
    struct timespec t2;
    int c;

    notzero(read(control_fd[0], &c, sizeof(c)), "control read failed");
    notzero(write(data_fd[1], &buffer_size, sizeof(buffer_size)), "writing length message failed");
    char buf[buffer_size];

    clock_gettime(CLOCK_MONOTONIC,  &t2);
    notzero(write(data_fd[1], &buf, buffer_size), "writing length message failed");
    write(data_fd[1], &t2, sizeof(struct timespec));
}

void test_lat(int iterations) {
    int sizes[] = {4, 16, 64, 256, 1*1024, 4*1024, 16*1024, 64*1024, 256*1024, 512*1024};
    long long min[10];
    long long result;
    int i, j;

    int data_fd[2];
    int control_fd[2];

    for (i= 0; i < 10; i++) {
        min[i] = DBL_MAX;
        for (j = 0; j < iterations; j++) {
            notzero(pipe(data_fd), "data pipe failed");
            notzero(pipe(control_fd), "control pipe failed");

            switch (fork()) {
            case 0:
                close(data_fd[0]);
                close(control_fd[1]);
                child(data_fd, control_fd, sizes[i]);
                exit(0);

            default: 
                close(data_fd[1]);
                close(control_fd[0]);

                result = parent(data_fd, control_fd);

                if (result < min[i]) 
                    min[i] = result;

                wait(NULL);

                close(data_fd[0]);
                close(control_fd[1]);
                continue;

            case -1:
                error("fork");
            }
        }
    }

    for (i = 0; i < 10; i++) {
        printf("%lld, ", min[i]);
    }

    printf("\nLatency -- %d iterations\n\n", iterations);
}

void test_throughput(int iterations, int kb) {
    int size = 1024 * 1024 * kb;
    long long min, result;
    int i, j;

    int data_fd[2];
    int control_fd[2];

    for (j = 0; j < iterations; j++) {
        notzero(pipe(data_fd), "data pipe failed");
        notzero(pipe(control_fd), "control pipe failed");

        switch (fork()) {
        case 0:
            close(data_fd[0]);
            close(control_fd[1]);
            child(data_fd, control_fd, size);
            exit(0);

        default: 
            close(data_fd[1]);
            close(control_fd[0]);

            result = parent(data_fd, control_fd);

            if (result < min) 
                min = result;

            wait(NULL);

            close(data_fd[0]);
            close(control_fd[1]);
            continue;

        case -1:
            error("fork");
        }
    }

    printf("%d %d\n", kb, min);
    printf("Thoughput -- %d iterations\n", iterations);
}

void main(int argc, char *argv[]) {
    int i = 1e2;

    // test_lat(i);
    test_throughput(i, 1);
}
