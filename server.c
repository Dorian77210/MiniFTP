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
#include <sys/stat.h>

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

void proceed_get_request(client_session session, request req)
{
    printf("Proceed get request \n");

    if (!send_file(session, req.path))
    {
        fprintf(stderr, "Error when send the file \n");
    }

    printf("End of the transmission of the get request \n");
}

void proceed_put_request(client_session session, request req)
{
    printf("Proceed put request \n");

    if (!store_file(session, req.path, req.nbbytes))
    {
        fprintf(stderr, "Error when store the file %s\n", req.path);
    }

    printf("End of the transmission of the put request \n");
}

void proceed_dir_request(client_session session, request req)
{
    int fds[2];

    int p = pipe(fds);
    if (p == -1)
    {
        perror("pipe");
        exit(1);
    }

    if(!fork()) {
        // create the pipe and replace the stdout with the pipe
        dup2(fds[1], 1);
        close(fds[0]);
        if(execl("/bin/ls", "ls", "-l", req.path, NULL) == -1) {
            fprintf(stderr, "Error when execl ls command \n");
            exit(1);
        }
    }

    close(fds[1]);

    block_t *block;
    char *buffer = (char *)malloc(9 * sizeof(char));
    if (!buffer)
    {
        fprintf(stderr, "Error when mallocing \n");
        exit(1);
    }

    int size = sizeof(block_t), n, buffer_size = 9;

    while (1)
    {
        memset(&block, 0, size);
        memset(buffer, 0, buffer_size);
        n = read(fds[0], buffer, size);
        if (n == -1)
        {
            perror("read");
            break;
        }

        if (n > 0)
        {
            int m = n;
            block = (block_t *)buffer;

            for(int i = n ; i < buffer_size; i++) {
                buffer[i] = '\0';
            }

            encrypt_block(block, session.session_key);

            n = send(session.sfd, buffer, buffer_size, 0x0);
            if (n == -1)
            {
                perror("send");
            }

        }
        else
        {
            printf("Send empty packet \n");
            // send empty packet
            n = send(session.sfd, NULL, 0, 0x0);
            if (n == -1)
            {
                perror("send");
            }

            break;
        }
    }

    free(buffer);

    close(fds[0]);
    close(fds[1]);

    printf("End of the dir request \n");
}