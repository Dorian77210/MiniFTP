#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "diffie_hellman.h"
#include "common.h"
#include "request.h"
#include "client.h"

#define GET_REQUEST_STRING "get"
#define PUT_REQUEST_STRING "put"
#define DIR_REQUEST_STRING "dir"

int main(int argc, const char** argv) {
    if(argc < 4) {
        fprintf(stderr, "Usage : %s <hostname> <request-type> <options...> \n", *argv);
        exit(1);
    }

    srand(time(NULL));

    int request_kind;

    // retrieve the good request
    if(!strcmp(argv[2], GET_REQUEST_STRING)) {
        if(argc != 5) {
            fprintf(stderr, "For the get request, please specify the distfilename and the localfilename \n");
            exit(2);
        }

        request_kind = REQUEST_GET;
    } else if(!strcmp(argv[2], PUT_REQUEST_STRING)) {
        if(argc != 5) {
            fprintf(stderr, "For the put request, please specify the distfilename and the localfilename \n");
            exit(3);
        }

        request_kind = REQUEST_PUT;
    } else if(!strcmp(argv[2], DIR_REQUEST_STRING)) {
        request_kind = REQUEST_DIR;
    } else {
        fprintf(stderr, "Bad request type value. Please choose among \"get\", \"put\" or \"dir\" \n");
        exit(5);
    }

    int r, n, sfd, size = sizeof(struct sockaddr_storage), temp = size;
    struct sockaddr_storage server;
    struct addrinfo* res, criteria, s;
    client_session session;

    // configuration of the criteria
    configure_criteria(&criteria);

    // retrieve information of server
    r = getaddrinfo(argv[1], SERVICE, &criteria, &res);
    if(r != 0) {
        fprintf(stderr, "getaddrinfo fails : %s \n", gai_strerror(r));
        exit(6);
    }

    // retrieve valid socket
    sfd = try_socket(&res, &s);
    if(sfd == -1) {
        fprintf(stderr, "socket binding fails \n");
        exit(7);
    }

    // retrieve the server
    memset(&server, 0, size);
    server = *((struct sockaddr_storage*)(s.ai_addr));
    freeaddrinfo(res);

    // establish connection
    if(connect(sfd, (struct sockaddr*)&server, size) == -1) {
        perror("connect");
        exit(8);
    }

    printf("Connected to the server. Ready for the transmission of data \n");

    // exchange of the key
    memset(&session, 0, sizeof(client_session));
    session = exchange_key(sfd, IS_CLIENT_KIND);

    // proceed the request
    if(request_kind == REQUEST_PUT) {
        proceed_put_request(session, argv[3], argv[4]);
    } else if(request_kind == REQUEST_GET) {

    } else if(request_kind == REQUEST_DIR) {

    }


    return EXIT_SUCCESS;
}