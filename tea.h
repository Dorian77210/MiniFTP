#ifndef TEA_H
#define TEA_H

#include "common.h"

void encrypt(uint32* v, uint32* k);
void decrypt(uint32* v, uint32* k);
void encrypt_block(block_t* block, uint128 k);
void decrypt_block(block_t* block, uint128 k);

#endif