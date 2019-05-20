#ifndef REQUEST_H
#define REQUEST_H

#define MAXPATH 256
#define REQUEST_PUT 1
#define REQUEST_GET 2
#define REQUEST_DIR 3

typedef struct {
    int kind;
    char path[MAXPATH];
    int nbbytes;
} request;

void send_request(request* r);

#endif