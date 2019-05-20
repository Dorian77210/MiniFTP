#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

int try_socket(struct addrinfo** res, struct addrinfo* s);

#endif