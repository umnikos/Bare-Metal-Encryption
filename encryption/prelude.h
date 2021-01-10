#pragma once
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

#include "../tiny-bignum-c/bn.h"
#include "../tiny-bignum-c/bn.c"
#define ii struct bn

#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)

#define i uint32_t
#define byte uint8_t
#define true 1
#define false 0
