#ifndef ANSWER_H
#define ANSWER_H

#define ANSWER_OK      0
#define ANSWER_UNKNOWN 1        /* requête inconnue */
#define ANSWER_ERROR   2        /* erreur lors du traitement */

#include "request.h"
#include "common.h"

void create_answer(request req, answer* ans);

#endif