#include "prelude.h"

void gcd(ii* a_in, ii* b_in, ii* res) {
  ii* a = res; bignum_assign(a, a_in);
  ii b_var; ii* b = &b_var; bignum_assign(b, b_in);
  mkii(temp);
  while (!bignum_is_zero(b)) {
    bignum_mod(a, b, temp);   // temp = a % b
    bignum_assign(a, b);      // a = b
    bignum_assign(b, temp);   // b = temp
  }
}

void gcd_extended(ii* a_in, ii* b_in, ii* res, ii* x, ii* y) {
  // TODO
}

void lcm(ii* a, ii* b, ii* res) {
  mkii(top);
  bignum_mul(a, b, top);
  mkii(bot);
  gcd(a, b, bot);
  bignum_div(top, bot, res);
}
