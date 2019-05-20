#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>

#include "server.h"
#include "common.h"

#define BACKLOG 64

int main(int argc, const char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage : %s <host> \n", *argv);
        exit(1);
    }

    // initialization of the variables
    struct sockaddr_storage server, client;
    struct addrinfo* res, criteria, s;
    int r, n, sfd, size = sizeof(struct sockaddr_storage), temp = size, newsfd;

    // configuration of the criteria
    configure_criteria(&criteria);

    // retrieve information with criteria
    r = getaddrinfo(NULL, argv[1], &criteria, &res);
    if(r != 0) {
        fprintf(stderr, "getaddrinfo fails : %s \n", gai_strerror(r));
        exit(2);
    }

    // retrieve valid socket
    sfd = socket_bind(&res, &s);
    if(sfd == -1) {
        fprintf(stderr, "socket bind fails \n");
        exit(3);
    }

    // retrieve the server 
    memset(&server, 0, size);
    server = *((struct sockaddr_storage*)(s.ai_addr));
    freeaddrinfo(res);

    // listening part
    if(listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(4);
    }

    printf("Yes\n");

    return EXIT_SUCCESS;
}