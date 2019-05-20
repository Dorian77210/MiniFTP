#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#define PORT 55554
#define SERVICE "55554"

typedef unsigned int uint32;
typedef unsigned __int128 uint128;
typedef unsigned long int uint64;


typedef struct {
    uint32* session_key;
} client_session;

void configure_criteria(struct addrinfo* criteria);

#endif