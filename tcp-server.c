#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "tests.h"

void serve_lat() {
  int parentfd, childfd, clientlen, optval;
  struct sockaddr_in serveraddr; 
  struct sockaddr_in clientaddr;
  struct hostent *hostp; 
  int sz;

  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  notzero(parentfd, "socket failed");

  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
  setsockopt(parentfd, IPPROTO_TCP, TCP_NODELAY|TCP_QUICKACK, (char *) &optval, sizeof(int)); 

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short) PORT);

  notzero(bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)), "bind failed");
  notzero(listen(parentfd, 5), "listen failed");
  clientlen = sizeof(clientaddr);

  while (1) {
    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    notzero(childfd, "accept failed");
    
    notzero(read(childfd, &sz, sizeof(int)), "control message failed");
    notzero(write(childfd, &sz, sizeof(int)), "control message failed");

    char buf[sz]; 
    notzero(read(childfd, buf, sz), "read failed"); 
    notzero(write(childfd, buf, sz), "write failed");

    close(childfd);
  }
}

void serve_through() {
  int parentfd, childfd, clientlen, optval;
  struct sockaddr_in serveraddr; 
  struct sockaddr_in clientaddr;
  struct hostent *hostp; 
  int sz;

  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  notzero(parentfd, "socket failed");

  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
  setsockopt(parentfd, IPPROTO_TCP, TCP_NODELAY|TCP_QUICKACK, (char *) &optval, sizeof(int)); 

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short) PORT);

  notzero(bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)), "bind failed");
  notzero(listen(parentfd, 5), "listen failed");
  clientlen = sizeof(clientaddr);

  while (1) {
    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    notzero(childfd, "accept failed");
    
    notzero(read(childfd, &sz, sizeof(int)), "control message failed");
    notzero(write(childfd, &sz, sizeof(int)), "control message failed");

    char buf[sz]; 
    notzero(read(childfd, buf, sz), "read failed"); 
    notzero(write(childfd, &sz, sizeof(int)), "control message failed");
    close(childfd);
  }
}

int main(int argc, char **argv) {
    serve_lat();
    // serve_through();
}