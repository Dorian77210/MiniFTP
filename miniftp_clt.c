#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "diffie_hellman.h"
#include "common.h"
#include "request.h"

#define GET_REQUEST_STRING "get"
#define PUT_REQUEST_STRING "put"
#define DIR_REQUEST_STRING "dir"

int main(int argc, const char** argv) {
    if(argc < 4) {
        fprintf(stderr, "Usage : %s <hostname> <request-type> <options...> \n", *argv);
        exit(1);
    }

    srand(time(NULL));

    int request_kind;
    request client_request;

    // retrieve the good request
    if(!strcmp(argv[2], GET_REQUEST_STRING)) {
        request_kind = REQUEST_GET;
    } else if(!strcmp(argv[2], PUT_REQUEST_STRING)) {
        request_kind = REQUEST_PUT;
    } else if(!strcmp(argv[2], DIR_REQUEST_STRING)) {
        request_kind = REQUEST_DIR;
    } else {
        fprintf(stderr, "Bad request type value. Please choose among \"get\", \"put\" or \"dir\" \n");
        exit(2);
    }

    client_request.kind = request_kind;



    // uint64 a, b;
    // uint64 g = generator();

    // uint64 k = create_part_key(g, &a);
    // uint64 buf = create_part_key(g, &b);

    // uint128 key = assembly_key(a, buf);
    // printf("%lu ", (unsigned long)key);
    // printf("%lu \n", (unsigned long)(key >> 64));

    return EXIT_SUCCESS;
}