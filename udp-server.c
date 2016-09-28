#include "tests.h"

void serve_lat() {
    int sock, clientlen, sz, optval;
    struct sockaddr_in serveraddr, clientaddr; 
    struct hostent *hostp; 

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    notzero(sock, "socket failed");

    optval = 1;
    notzero(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)), "setsockopt faild");

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)PORT);
    clientlen = sizeof(clientaddr);

    notzero(bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)), "bind failed");

    while (1) {
        notzero(recvfrom(sock, &sz, sizeof(int), 0,(struct sockaddr *) &clientaddr, &clientlen), "recv control failed");
        notzero(sendto(sock, &sz, sizeof(int), 0, (struct sockaddr *) &clientaddr, clientlen), "send control failed");

        char buf[sz]; 
        bzero(buf, sz);

        recvfrom(sock, buf, sz, 0,(struct sockaddr *) &clientaddr, &clientlen);
        sendto(sock, buf, sz, 0, (struct sockaddr *) &clientaddr, clientlen);
    }
}

void serve_through() {
    int sock, clientlen, sz, num, optval;
    struct sockaddr_in serveraddr, clientaddr; 
    struct hostent *hostp; 

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    notzero(sock, "socket failed");

    optval = 1;
    notzero(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)), "setsockopt faild");

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)PORT);
    clientlen = sizeof(clientaddr);

    notzero(bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)), "bind failed");

    while (1) {
        notzero(recvfrom(sock, &sz, sizeof(sz), 0,(struct sockaddr *) &clientaddr, &clientlen), "recv control failed");
        notzero(recvfrom(sock, &num, sizeof(num), 0,(struct sockaddr *) &clientaddr, &clientlen), "recv control failed");
        notzero(sendto(sock, &sz, sizeof(sz), 0, (struct sockaddr *) &clientaddr, clientlen), "send control failed");
        char buf[sz]; 

        for (int i = 0; i < num; i++) {
            recvfrom(sock, &buf, num, 0,(struct sockaddr *) &clientaddr, &clientlen);

            if (buf[0] == '\0') 
                break;
        }

        printf("Done: %d chunks of length %d\n", num, sz);
        notzero(sendto(sock, &sz, sizeof(int), 0, (struct sockaddr *) &clientaddr, clientlen), "send control failed");
    }
}


void main(int argc, char **argv) {
    // serve_lat();
    serve_through();
}