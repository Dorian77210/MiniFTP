#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "request.h"
#include "common.h"
#include "tea.h"
#include "answer.h"

void crypt_request(uint128 key, request* r) {
    int size = sizeof(r);
    uint32* raw_key = (uint32*)&key;
    uint32* req = (uint32*)r;
    
    for(int i = 0; i < size; i += 2) {
        encrypt(&req[i], raw_key);
    }
}

void decrypt_request(uint128 key, request* r) {
    int size = sizeof(r);
    uint32* req = (uint32*)r;
    uint32* raw_key = (uint32*)&key;

    for(int i = 0; i < size; i += 2) {
        decrypt(&req[i], raw_key);
    }
}

answer send_request(int sfd, request* r) {
    answer ans;
    int n;

    // send the request
    n = send(sfd, r, sizeof(request), 0x0);
    if(n == -1) {
        perror("send");
        exit(1);
    }

    // receive the answer
    n = recv(sfd, &ans, sizeof(ans), 0x0);
    if(n == -1) {
        perror("recv");
        exit(2);
    }

    return ans;
}

client_session exchange_key(int sfd, int kind) {
    int size = sizeof(uint64);
    uint64 g = generator();
    uint64 power, buffer;
    client_session session;
    memset(&session, 0, sizeof(client_session));
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

    uint128 raw_key = assembly_key(power, buffer);
    session.sfd = sfd;
    session.session_key = raw_key;

    return session;
}