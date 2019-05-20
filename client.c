#include <stdlib.h>

#include "client.h"

int try_socket(struct addrinfo** res, struct addrinfo* s) {
    int sfd;
    struct addrinfo* resp = *res;
    while(resp != NULL) {
        // try to create the socket
        sfd = socket(resp->ai_family, resp->ai_socktype, resp->ai_protocol);
        if(sfd != -1) {
            *s = *resp;
            break;
        }
    }

    return sfd;
}