#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "diffie_hellman.h"

#define SOPHIE_GERMAIN_P 4611686018427402023

uint64 expm(uint64 m, uint64 e, uint64 mod)
{
    uint128 _r = 1U;
    uint128 _m = (uint128)m;
    uint128 _mod = (uint128)mod;
    while (e)
    {
        if (e & 1)
            _r = (_r * _m) % _mod;
        _m = (_m * _m) % _mod;
        e >>= 1;
    }
    return (uint64)_r;
}

uint64 generator()
{
    uint64 p = SOPHIE_GERMAIN_P;
    uint64 q = (p - 1) / 2, temp;
    for (uint64 g = 2; g < p; g++)
    {
        temp = expm(g, q, p);
        if (temp != 1)
            return g;
    }

    return 0;
}

uint64 create_part_key(uint64 g, uint64* a) {
    // generate random number
    uint64 p = SOPHIE_GERMAIN_P;
    *a = 2 + rand() % ((p - 2 + 1));
    return expm(g, *a, p);
}

uint128 assembly_key(uint64 a, uint64 b) {
    uint64 p = SOPHIE_GERMAIN_P;
    uint64 res = expm(b, a, p);

    uint128 key = (uint128)res;

    return (key | key << 64);
}

void print_key(uint128 key) {
    uint32* raw_key = (uint32*)&key;
    for(int i = 0; i < 4; i++) {
        printf("%u \n", raw_key[i]);
    }
}