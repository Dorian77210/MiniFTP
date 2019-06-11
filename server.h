#ifndef SERVER_H
#define SERVER_H

#define MAX_COMMAND_SIZE 512

#include "common.h"

int socket_bind(struct addrinfo** res, struct addrinfo* s);
void proceed_put_request(client_session session, request req);
void proceed_dir_request(client_session session, request req);
void proceed_get_request(client_session session, request req);

#endif