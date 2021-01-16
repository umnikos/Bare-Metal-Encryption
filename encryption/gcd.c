#include "prelude.h"

void gcd(ii* a, ii* b, ii* res) {
  bignum_assign(res, a);
  while (!bignum_is_zero(b)) {
    bignum_mod(res, b, a);   // temp = a % b
    bignum_assign(res, b);   // a = b
    bignum_assign(b, a);     // b = temp
  }
}
