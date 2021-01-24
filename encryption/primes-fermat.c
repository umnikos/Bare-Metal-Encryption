#include "prelude.h"

#define fermat_iterations 50

byte is_prime_fermat(ii* p) {
  mkii(p_minus_one);
  bignum_assign(p_minus_one, p);
  bignum_dec(p_minus_one);
  mkii(temp);
  mkii(n);
  for (i j=0; j<fermat_iterations; j++) {
    i rn = rng();
    //printf("%u\n", rn);
    bignum_from_int(n, rn);
    modular_exponentiation(n, p_minus_one, p, temp);
    bignum_dec(temp);
    if (!bignum_is_zero(temp)) {
      return false;
    }
  }
  return true;
}

void generate_prime_fermat(i size, ii* res) {
  do {
    mkii(temp1);
    mkii(temp2);
    bignum_init(res);
    for (i j=0; j<size; j++) {
      bignum_lshift(res, temp1, 32);
      bignum_from_int(temp2, rng());
      bignum_or(temp1, temp2, res);
    }
    printf(".");
    fflush(stdout);
  } while (!is_prime_fermat(res));
}


