#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "answer.h"
#include "request.h"

void create_answer(request req, answer* ans) {
    int kind = req.kind;

    if(kind == REQUEST_DIR) {
        create_dir_request(req, ans);
    } else if(kind == REQUEST_GET) {
        create_get_request(req, ans);
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
    int fd = open(req.path, O_CREAT | O_TRUNC, 00777);
    if(fd == -1) {
        ans->ack = ANSWER_ERROR;
        ans->errnum = errno;
    }

    close(fd);
}

void create_get_answer(request req, answer* ans) {

}

void create_dir_answer(request req, answer* ans) {

}