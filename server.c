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
#include "tea.h"
#include "common.h"

int socket_bind(struct addrinfo **res, struct addrinfo *s)
{
    struct addrinfo *resp = *res;
    int sfd;

    while (resp != NULL)
    {
        // try to bind this element as a server
        sfd = socket(resp->ai_family, resp->ai_socktype, resp->ai_protocol);
        if (sfd != -1)
        {
            // try to bind
            int bind_resp = bind(sfd, resp->ai_addr, resp->ai_addrlen);
            if (bind_resp != -1)
            {
                *s = *resp;
                return sfd;
            }
        }
        else
        {
            return -1;
        }

        resp = resp->ai_next;
    }

    return -1;
}

void proceed_put_request(client_session session, request req)
{
    printf("Proceed put request \n");

    // open the file
    int fd = open(req.path, O_WRONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    int filesize = req.nbbytes;

    int blocks_count = filesize / BLOCK_SIZE, n, m;
    block_t block;
    int size = sizeof(block);
    char *buffer = NULL;

    for (int i = 0; i < blocks_count; i++)
    {
        memset(&block, 0, size);
        n = recv(session.sfd, &block, size, 0x0);
        if (n == -1)
        {
            fprintf(stderr, "Error when receiving data \n");
            perror("recv");
        }

        decrypt_block(&block, session.session_key);
        buffer = (char *)&block;

        if (i == (blocks_count - 1))
        {
            // last block, retrieve the padding
            int padding = block & 0xFF;
            int bytes_to_write = BLOCK_SIZE - padding;
            if (is_big_endian())
            {
                if (bytes_to_write != 0)
                {
                    printf("Write extra bytes by padding \n");
                    m = write(fd, buffer + padding, bytes_to_write);
                    if (m == -1)
                    {
                        perror("write");
                        close(fd);
                        return;
                    }
                }
            }
            else
            {
                // TODO, no implemented yet
            }
        }
        else
        {
            m = write(fd, &block, n);
            if (m == -1)
            {
                perror("write");
                close(fd);
                return;
            }
        }
    }

    printf("End of the transmission of the put request \n");
    close(fd);
}