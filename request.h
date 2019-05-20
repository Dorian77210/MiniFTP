#ifndef REQUEST_H
#define REQUEST_H

#define MAXPATH 256
#define REQUEST_PUT 1
#define REQUEST_GET 2
#define REQUEST_DIR 3

#include "tea.h"
#include "diffie_hellman.h"
#include "common.h"

typedef struct {
    int kind;
    char path[MAXPATH];
    int nbbytes;
} request;

void send_request(request* r);
session exchange_key(int sfd, int kind);

#endif