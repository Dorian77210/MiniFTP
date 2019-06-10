#ifndef ANSWER_H
#define ANSWER_H

#define ANSWER_OK      0
#define ANSWER_UNKNOWN 1        /* requête inconnue */
#define ANSWER_ERROR   2        /* erreur lors du traitement */

#include "request.h"
#include "common.h"

void create_answer(request req, answer* ans);
void create_put_answer(request req, answer* ans);
void create_get_answer(request req, answer* ans);
void create_dir_answer(request req, answer* ans);
void send_answer(int sfd, answer* ans);

void crypt_answer(uint128 key, answer* ans);
void decrypt_answer(uint128 key, answer* ans);

#endif