#include "prelude.h"

void gcd(ii* a_in, ii* b_in, ii* res) {
  ii* a = res; bignum_assign(a, a_in);
  ii b_var; ii* b = &b_var; bignum_assign(b, b_in);
  ii temp_var; ii* temp = &temp_var;
  while (!bignum_is_zero(b)) {
    bignum_mod(a, b, temp);   // temp = a % b
    bignum_assign(a, b);      // a = b
    bignum_assign(b, temp);   // b = temp
  }
}
