#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#include "common.h"
#include "request.h"

int try_socket(struct addrinfo** res, struct addrinfo* s);
void proceed_put_request(client_session session, const char* localfilename, const char* destfilename);

#endif