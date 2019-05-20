#include "request.h"

#define IS_SERVER_KIND 1
#define IS_CLIENT_KIND 2

void send_request(request* r) {
    
}

session exchange_key(int sfd, int kind) {
    int size = sizeof(uint64);
    uint64 g = generator();
    uint64* power, buffer;
    session s;
    int n;

    // generate the part of key
    uint64 part_key = create_part_key(g, &power);
    if(kind == IS_SERVER_KIND) {
        // receive from Alice, then send the part_key
        n = recv(sfd, &buffer, size, 0x0);
        if(n == -1) {
            perror("recv");
            close(sfd);
            exit(1);
        }

        n = send(sfd, &part_key, size, 0x0);
        if(n == -1) {
            perror("recv");
            close(sfd);
            exit(2);
        }

    } else if(kind == IS_CLIENT_KIND) {
        // send from Alice, then receive the part_key
        n = send(sfd, &part_key, size, 0x0);
        if(n == -1) {
            perror("recv");
            close(sfd);
            exit(3);
        }

        n = recv(sfd, &buffer, size, 0x0);
        if(n == -1) {
            perror("recv");
            close(sfd);
            exit(3);
        }
    } else {
        fprintf(stderr, "Bad value for the kind of sender/receiver \n");
        exit(4);
    }

    uint128 key = assembly_key(power, buffer);
    s.session_key = key;

    return s;
}   