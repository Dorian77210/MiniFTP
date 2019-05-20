#ifndef REQUEST_H
#define REQUEST_H

#define MAXPATH 256
#define REQUEST_PUT 1
#define REQUEST_GET 2
#define REQUEST_DIR 3

#define IS_SERVER_KIND 1
#define IS_CLIENT_KIND 2

#include "tea.h"
#include "diffie_hellman.h"
#include "common.h"

typedef struct {
    int kind;
    char path[MAXPATH];
    int nbbytes;
} request;

void send_request(request* r);
client_session exchange_key(int sfd, int kind);

#endif