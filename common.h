#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

typedef unsigned int uint32;
typedef unsigned __int128 uint128;
typedef unsigned long int uint64;


typedef struct {

} session;

void configure_criteria(struct addrinfo* criteria);

#endif