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
#include "common.h"
#include "request.h"

#define BACKLOG 64

int main(int argc, const char** argv) {
    // initialization of the variables
    struct sockaddr_storage server, client;
    struct addrinfo* res, criteria, s;
    int r, n, sfd, size = sizeof(struct sockaddr_storage), temp = size;
    request req;
    answer ans;

    // configuration of the criteria
    configure_criteria(&criteria);

    // retrieve information with criteria
    r = getaddrinfo(NULL, SERVICE, &criteria, &res);
    if(r != 0) {
        fprintf(stderr, "getaddrinfo fails : %s \n", gai_strerror(r));
        exit(2);
    }

    // retrieve valid socket
    sfd = socket_bind(&res, &s);
    if(sfd == -1) {
        fprintf(stderr, "socket bind fails \n");
        exit(3);
    }

    // retrieve the server
    memset(&server, 0, size);
    server = *((struct sockaddr_storage*)(s.ai_addr));
    freeaddrinfo(res);

    // listening part
    if(listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(4);
    }

    while(1) {
        memset(&client, 0, size);
        int newsfd = accept(sfd, (struct sockaddr*)&client, &temp);
        if(newsfd == -1) {
            perror("accept");
            close(sfd);
            return EXIT_FAILURE;
        }

        if(!fork()) {
            close(sfd);
            printf("Receive a new client. Ready for the transmission of data \n");
            client_session session;

            // memset on variables
            memset(&session, 0, sizeof(session));
            memset(&ans, 0, sizeof(answer));
            memset(&req, 0, sizeof(request));

            // retrieve the session
            session = exchange_key(newsfd, IS_SERVER_KIND);

            // receive the request of the client
            n = recv(newsfd, &req, sizeof(request), 0x0);
            if(n == -1) {
                perror("recv");
                exit(6);
            }

            decrypt_request(session.session_key, &req);
            // create the answer and send the answer to the client
            create_answer(req, &ans);
            int ack = ans.ack;

            // crypt the answer
            crypt_answer(session.session_key, &ans);
            // send the answer to to the client
            send_answer(session.sfd, &ans);
            
            if(ack == ANSWER_OK) {
                // proceed the request
                if(req.kind == REQUEST_PUT) {
                    proceed_put_request(session, req);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}