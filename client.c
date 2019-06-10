#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "client.h"
#include "request.h"
#include "common.h"
#include "answer.h"


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

void proceed_put_request(client_session session, const char* localfilename, const char* destfilename) {
    request req;
    answer ans;
    struct stat stats;
    uint32 block[2]; // block
    size_t bytes_read;
    int dest_size = strlen(destfilename), n;

    req.kind = REQUEST_PUT;

    // retrieve the size of the file
    int local_fd = open(localfilename, O_RDONLY);
    if(local_fd == -1) {
        perror("open");
        exit(1);
    }

    // int dest_fd = open(destfilename, O_TRUNC | O_CREAT | O_WRONLY);
    // if(dest_fd == -1) {
    //     perror("open");
    //     exit(2);
    // }

    int filesize = fstat(local_fd, &stats);
    if(filesize == -1) {
        perror("fstat");
        exit(3);
    }

    req.nbbytes = filesize;
    // put the filename in the structure
    strncpy(req.path, destfilename, dest_size);

    // encrypt and send the request
    crypt_request(session.session_key, &req);
    ans = send_request(session.sfd, &req);
    // decrypt the answer
    decrypt_answer(session.session_key, &ans);

    close(local_fd);

    if(ans.ack == ANSWER_ERROR) {
        fprintf(stderr, "Answer Error");
    } else if(ans.ack == ANSWER_OK) {
        // will send the file
        printf("Answer ok \n");
        // send the file to the server
    } else if(ans.ack == ANSWER_UNKNOWN) {
        fprintf(stderr, "request unknown \n");
        exit(1);
    }

    // decrypt the answer

    // close(dest_fd);
}