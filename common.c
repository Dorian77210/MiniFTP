#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"

void configure_criteria(struct addrinfo* criteria) {
    // configuration of the criteria
    memset(criteria, 0, sizeof(struct addrinfo));
    criteria->ai_protocol = 0x0;
    criteria->ai_socktype = SOCK_STREAM;
    criteria->ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    criteria->ai_family = AF_UNSPEC;
    criteria->ai_addr = NULL;
    criteria->ai_canonname = NULL;
    criteria->ai_next = NULL;
}