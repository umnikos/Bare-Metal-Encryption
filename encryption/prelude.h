#pragma once
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

#include "../tiny-bignum-c/bn.h"
#define ii struct bn
#define II_BITCOUNT BN_ARRAY_SIZE*WORD_SIZE

#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)

#define i uint32_t
#define byte uint8_t
#define true 1
#define false 0
