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
#include "request.h"


int socket_bind(struct addrinfo** res, struct addrinfo* s) {
    struct addrinfo* resp = *res;
    int sfd;

    while(resp != NULL) {
        // try to bind this element as a server
        sfd = socket(resp->ai_family, resp->ai_socktype, resp->ai_protocol);
        if(sfd != -1) {
            // try to bind
            int bind_resp = bind(sfd, resp->ai_addr, resp->ai_addrlen);
            if(bind_resp != -1) {
                *s = *resp;
                return sfd;
            }
        } else {
            return -1;
        }

        resp = resp->ai_next;
    }

    return -1;
}

void proceed_put_request(client_session session, request req) {
    printf("Proceed put request \n");
}