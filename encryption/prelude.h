#pragma once
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

#define WORD_SIZE 4
#include "../tiny-bignum-c/bn.h"
#define ii struct bn
#define II_BITCOUNT BN_ARRAY_SIZE*WORD_SIZE
#define mkii(V) ii V##_var; ii* V = & V##_var
#define inii(V) mkii(V); bignum_assign(V, V##_in);

#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)

#define i uint32_t
#define byte uint8_t
#define true 1
#define false 0

void print_bignum(ii* n);
void exponentiation(ii* b, ii* e, ii* res);
void modular_exponentiation(ii* b, ii* e, ii* m, ii* res);
void gcd(ii* a, ii* b, ii* res);
void lcm(ii* a, ii* b, ii* res);
void init_rng();
i rng();
void generate_prime_fermat(ii* res);
