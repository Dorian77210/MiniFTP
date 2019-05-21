#ifndef REQUEST_H
#define REQUEST_H

#define REQUEST_PUT 1
#define REQUEST_GET 2
#define REQUEST_DIR 3

#define IS_SERVER_KIND 1
#define IS_CLIENT_KIND 2

#include "tea.h"
#include "diffie_hellman.h"
#include "common.h"
#include "answer.h"

void crypt_request(uint128 key, request* r);
void decrypt_request(uint128 key, request* r);
answer send_request(int sfd, request* r);
client_session exchange_key(int sfd, int kind);

#endif