#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "answer.h"
#include "request.h"

void create_answer(request req, answer* ans) {
    int kind = req.kind;

    if(kind == REQUEST_DIR) {
        create_dir_answer(req, ans);
    } else if(kind == REQUEST_GET) {
        create_get_answer(req, ans);
    } else if(kind == REQUEST_PUT) {
        create_put_answer(req, ans);
    } else {
        ans->ack = ANSWER_UNKNOWN;
    }
}

// privates functions
void create_put_answer(request req, answer* ans) {
    ans->ack = ANSWER_OK;
    
    // create the file
    int fd = open(req.path, O_CREAT | O_TRUNC, 00666);
    if(fd == -1) {
        ans->ack = ANSWER_ERROR;
        ans->errnum = errno;
    } else {
        ans->ack = ANSWER_OK;
    }

    close(fd);
}

void create_get_answer(request req, answer* ans) {
    // open the wanted file 
    int fd = open(req.path, O_RDONLY);
    if(fd == -1) {
        ans->ack = ANSWER_ERROR;
        ans->errnum = errno;
    } else {
        // retrieve the size of the file
        struct stat stats;
        int s = fstat(fd, &stats);
        if(s == -1) {
            perror("fstat");
            ans->ack = ANSWER_ERROR;
            ans->errnum = errno;
        } else {
            ans->ack = ANSWER_OK;
            int filesize = stats.st_size;
            // size with the future padding
            filesize += !(stats.st_size % BLOCK_SIZE) ? BLOCK_SIZE : BLOCK_SIZE - (stats.st_size % BLOCK_SIZE);
            ans->nbbytes = filesize;
        }

        close(fd);
    }
}

void create_dir_answer(request req, answer* ans) {
    // try to open the folder
    DIR* dir = opendir(req.path);

    if(!dir) {
        ans->ack = ANSWER_ERROR;
        ans->errnum = errno;
    } else {
        ans->ack = ANSWER_OK;
    }

    closedir(dir);
}

void send_answer(int sfd, answer* ans) {
    int n = send(sfd, ans, sizeof(ans), 0x0);
    if(n == -1) {
        fprintf(stderr, "Error when send the answer to the client \n");
    }
}

void crypt_answer(uint128 key, answer* ans) {
    int size = sizeof(ans);
    uint32* raw_key = (uint32*)&key;
    uint32* req = (uint32*)ans;
    
    for(int i = 0; i < size; i += 2) {
        encrypt(&req[i], raw_key);
    }
}

void decrypt_answer(uint128 key, answer* ans) {
    int size = sizeof(ans);
    uint32* req = (uint32*)ans;
    uint32* raw_key = (uint32*)&key;

    for(int i = 0; i < size; i += 2) {
        decrypt(&req[i], raw_key);
    }
}