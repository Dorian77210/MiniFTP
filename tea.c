#include "tea.h"

#define TEA_MAX_ITERATIONS 32
#define DELTA 0x9e3779b9

void encrypt (uint32* v, uint32* k) {
    uint32 v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32 k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < TEA_MAX_ITERATIONS; i++) {                       /* basic cycle start */
        sum += DELTA;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

void decrypt(uint32* v, uint32* k) {
    uint32 v0 = v[0], v1 = v[1], sum = DELTA * TEA_MAX_ITERATIONS, i;           /* set up */
    uint32 k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
    for (i=0; i < TEA_MAX_ITERATIONS; i++) {                       /* basic cycle start */
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= DELTA;
    }                                              /* end cycle */
    v[0] = v0; v[1] = v1;
}