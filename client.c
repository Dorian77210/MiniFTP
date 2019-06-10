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
        printf("Answer ok \n");
        // send the file to the server
        send_file(session, localfilename, stats.st_size);
    }
    else if (ans.ack == ANSWER_UNKNOWN)
    {
        fprintf(stderr, "request unknown \n");
        exit(1);
    }
}


// privates functions

int send_file(client_session session, const char *filename, int filesize)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    int blocks_count = (filesize / BLOCK_SIZE), n;
    block_t block;
    int size = sizeof(block_t);

    int add_block = (filesize % BLOCK_SIZE) == 0;
    blocks_count += add_block ? 0 : 1;

    for (int i = 0; i < blocks_count; i++)
    {
        memset(&block, 0, size);
        n = read(fd, &block, size);

        if (n == -1)
        {
            perror("read");
            close(fd);
            return 0;
        }

        if (i == (blocks_count - 1))
        {
            if (add_block)
            {   
                encrypt_block(&block, session.session_key);
                // send the block
                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }

                // add a last block
                memset(&block, 0, size);
                block = 0x8;
                encrypt_block(&block, session.session_key);

                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }
            }
            else
            {
                int shift = BLOCK_SIZE - n;
                block = block << (shift * 8);
                block |= shift;

                encrypt_block(&block, session.session_key);
                // send the block
                n = send(session.sfd, &block, BLOCK_SIZE, 0x0);
                if(n == -1) {
                    perror("send");
                }
            }
        } else {
            encrypt_block(&block, session.session_key);
            n = send(session.sfd, &block, n, 0x0);
            if(n == -1) {
                perror("send");
            }
        }
    }

    close(fd);
}