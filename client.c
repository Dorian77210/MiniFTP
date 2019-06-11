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
#include "tea.h"

int try_socket(struct addrinfo **res, struct addrinfo *s)
{
    int sfd;
    struct addrinfo *resp = *res;
    while (resp != NULL)
    {
        // try to create the socket
        sfd = socket(resp->ai_family, resp->ai_socktype, resp->ai_protocol);
        if (sfd != -1)
        {
            *s = *resp;
            break;
        }
    }

    return sfd;
}

void proceed_get_request(client_session session, const char* destfilename, const char* localfilename) {
    request req;
    answer ans;

    req.kind = REQUEST_GET;
    int dest_size = strlen(destfilename);

    // set the path of the wanted file
    memset(req.path, 0, MAXPATH);
    strncpy(req.path, destfilename, dest_size);

    crypt_request(session.session_key, &req);
    ans = send_request(session.sfd, &req);

    // decrypt the answer
    decrypt_answer(session.session_key, &ans);

    if(ans.ack == ANSWER_ERROR) {
        fprintf(stderr, "Answer error for the get request \n");
    } else if(ans.ack == ANSWER_UNKNOWN) {
        fprintf(stderr, "Answer unknown for the get request \n");
    } else if(ans.ack == ANSWER_OK) {
        printf("Ready for the get request \n");
        if(!store_file(session, localfilename, ans.nbbytes)) {
            fprintf(stderr, "Error when store the file on the server \n");
        }

        printf("End of the get request \n");
    }
}

void proceed_put_request(client_session session, const char *localfilename, const char *destfilename)
{
    request req;
    answer ans;
    struct stat stats;
    uint32 block[2]; // block
    size_t bytes_read;
    int dest_size = strlen(destfilename), n;

    req.kind = REQUEST_PUT;

    // retrieve the size of the file
    int local_fd = open(localfilename, O_RDONLY);
    if (local_fd == -1)
    {
        perror("open");
        exit(1);
    }

    int s = fstat(local_fd, &stats);
    if (s == -1)
    {
        perror("fstat");
        exit(3);
    }

    // calculate the real size of the file
    int filesize = stats.st_size;
    filesize += !(stats.st_size % BLOCK_SIZE) ? BLOCK_SIZE : BLOCK_SIZE - (stats.st_size % BLOCK_SIZE);

    req.nbbytes = filesize;
    // put the filename in the structure
    memset(req.path, 0, MAXPATH);
    strncpy(req.path, destfilename, dest_size);

    // create a copy of the request
    request copy = req;

    // encrypt and send the request
    crypt_request(session.session_key, &req);
    ans = send_request(session.sfd, &req);
    // decrypt the answer
    decrypt_answer(session.session_key, &ans);

    close(local_fd);

    if (ans.ack == ANSWER_ERROR)
    {
        fprintf(stderr, "Answer Error");
    }
    else if (ans.ack == ANSWER_OK)
    {
        // will send the file
        printf("Answer ok for put request\n");
        // send the file to the server
        if(!send_file(session, localfilename)) {
            fprintf(stderr, "Error when send the file to the server \n");
        }

        printf("End of the put request \n");
    }
    else if (ans.ack == ANSWER_UNKNOWN)
    {
        fprintf(stderr, "request unknown for put request\n");
        exit(1);
    }
}

void proceed_dir_request(client_session session, const char* dir) {
    request req;
    answer ans;

    int dest_size = strlen(dir);
    req.kind = REQUEST_DIR;

    // put the filename in the structure
    memset(req.path, 0, MAXPATH);
    strncpy(req.path, dir, dest_size);

    // encrypt and send the request

    crypt_request(session.session_key, &req);
    ans = send_request(session.sfd, &req);
    // decrypt the answer
    decrypt_answer(session.session_key, &ans);

    if (ans.ack == ANSWER_ERROR)
    {
        fprintf(stderr, "Answer Error during dir request\n");
    }
    else if (ans.ack == ANSWER_OK)
    {
        // will send the file
        printf("Answer ok for dir request\n");
        // receive ls from the server
        if(!receive_ls(session)) {
            fprintf(stderr, "Error when receiving the ls command \n");
        }

        printf("End of the dir request \n");
    }
    else if (ans.ack == ANSWER_UNKNOWN)
    {
        fprintf(stderr, "request unknown for dir request\n");
        exit(1);
    }
}

int receive_ls(client_session session) {
    char* buffer = (char*)malloc(9 * sizeof(char));
    if(!buffer) {
        fprintf(stderr, "Malloc error \n");
        return 0;
    }

    block_t* block;
    int n, size = sizeof(block_t);

    while(1) {
        memset(&block, 0, size);
        memset(buffer, 0, 9);
        n = recv(session.sfd, buffer, 9, 0x0);
        if(n == -1) {
            perror("recv");
            return 0;
        }

        if(!n) {
            // end of the transmission
            break;
        }

        block = (block_t*)buffer;
        decrypt_block(block, session.session_key);

        for(int i = n; n < 9; i++) {
            buffer[i] = '\0';
        }

        printf("%s", buffer);
        fflush(stdout);
    }

    free(buffer);
    return 1;
}