#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#include "common.h"
#include "request.h"

int try_socket(struct addrinfo** res, struct addrinfo* s);
void proceed_put_request(client_session session, const char* localfilename, const char* destfilename);
void proceed_get_request(client_session session, const char* destfilename, const char* localfilename);
void proceed_dir_request(client_session session, const char* dir);
int receive_ls(client_session session);

#endif