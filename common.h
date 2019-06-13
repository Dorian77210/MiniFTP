#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#define PORT 55555
#define BLOCK_SIZE 8
#define SERVICE "55555"
#define MAXPATH 256

typedef unsigned int uint32;
typedef unsigned __int128 uint128;
typedef unsigned long int uint64;
typedef uint64 block_t;

typedef struct {
    uint128 session_key;
    int sfd;
} client_session;

typedef struct {
	int  ack;
	int  nbbytes;  /* pour GET seulement */
	int  errnum;   /* significatif ssi != 0 et ack == ANSWER_ERROR */
	int _pad[1];   /* aligne la taille sur un multiple de 8 octests */
} answer;

typedef struct {
    int kind;
    char path[MAXPATH];
    int nbbytes;
} request;

void configure_criteria(struct addrinfo* criteria);
int is_little_endian();
int store_file(client_session session, const char* filename, int filesize);
int send_file(client_session session, const char* filename);

#endif