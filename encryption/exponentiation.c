#include "prelude.h"

// https://cp-algorithms.com/algebra/binary-exp.html
void exponentiation(ii* b_in, ii* e_in, ii* res) {
  inii(b);
  inii(e);

  mkii(temp);
  ii one; bignum_from_int(&one, 1);

  bignum_from_int(res, 1); // res = 1
  while (!bignum_is_zero(e)) {
    if (e->array[0]&1) { // if (e & 1) {
      bignum_mul(res, b, temp); bignum_assign(res, temp); // res *= b
    }
    bignum_mul(b, b, temp); bignum_assign(b, temp); // b *= b
    bignum_rshift(e, temp, 1); bignum_assign(e, temp); // e = e >> 1
  }
}

void modular_exponentiation(ii* b_in, ii* e_in, ii* m, ii* res) {
  inii(b);
  inii(e);

  mkii(temp);
  ii one; bignum_from_int(&one, 1);

  bignum_mod(b, m, temp); bignum_assign(b, temp); // b %= m
  bignum_from_int(res, 1); // res = 1
  while (!bignum_is_zero(e)) {
    if (e->array[0]&1) { // if (e & 1) {
      bignum_mul(res, b, temp); bignum_mod(temp, m, res); // res = (res * b) % m
    }
    bignum_mul(b, b, temp); bignum_mod(temp, m, b); // b = (b * b) % m
    bignum_rshift(e, temp, 1); bignum_assign(e, temp); // e = e >> 1
  }
}
