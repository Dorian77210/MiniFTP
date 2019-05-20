#ifndef DIFFIE_HELLMAN_H
#define DIFFIE_HELLMAN_H

#include "common.h"

uint64 expm(uint64 m, uint64 e, uint64 mod);
uint64 generator();
uint64 create_part_key(uint64 g, uint64* a);
uint128 assembly_key(uint64 a, uint64 b);

#endif