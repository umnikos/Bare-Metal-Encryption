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

// https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Pseudocode
// return x in ax+by=res
si gcd_extended(ii* a_in, ii* b_in, ii* res) {
  ii* a = res; bignum_assign(a, a_in);
  ii b_var; ii* b = &b_var; bignum_assign(b, b_in);
  mkii(q);
  mkii(temp1);
  mkii(temp2);
  si temp3;
  si x=1, xx=0;
  while (!bignum_is_zero(b)) {
    bignum_div(a, b, q);

    // a,b=b,a%b
    bignum_mul(q, b, temp1); // temp1 = q*oldb
    bignum_assign(temp2, a); // temp2 = olda
    bignum_assign(a, b); // newa = oldb
    bignum_sub(temp2,temp1,b); // newb = olda - q*oldb

    if (xx == 0) {
      temp3 = x;
      x = xx;
      xx = temp3;
    } else {
      // now we hope b was small or the answer will be wrong
      temp3 = x;
      x = xx;
      xx = temp3 - bignum_to_int(q) * xx;
    }
  }
  return x;
}

void lcm(ii* a, ii* b, ii* res) {
  mkii(top);
  bignum_mul(a, b, top);
  mkii(bot);
  gcd(a, b, bot);
  bignum_div(top, bot, res);
}
