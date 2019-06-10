#ifndef SERVER_H
#define SERVER_H

#include "common.h"

int socket_bind(struct addrinfo** res, struct addrinfo* s);
void proceed_put_request(client_session session, request req);

#endif