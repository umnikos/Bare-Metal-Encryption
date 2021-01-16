#include "prelude.h"

// https://cp-algorithms.com/algebra/binary-exp.html
void exponentiation(ii* b, ii* e, ii* res) {
  bignum_from_int(res, 1);
  ii one;
  bignum_from_int(&one, 1);
  ii temp;
  while (!bignum_is_zero(e)) {
    if (e->array[0]&1) { // if (e & 1) {
      bignum_mul(res, b, &temp); // res *= b
      bignum_assign(res, &temp);
    }
    bignum_mul(b, b, &temp); // b *= b
    bignum_assign(b, &temp);
    bignum_rshift(e, &temp, 1); // e = e >> 1
    bignum_assign(e, &temp);
  }
}

void modular_exponentiation(ii* b, ii* e, ii* m, ii* res) {
  // TODO
}
