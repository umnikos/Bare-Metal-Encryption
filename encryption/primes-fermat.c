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

void generate_prime_fermat(ii* res) {
  do {
    printf(".");
    fflush(stdout);
    bignum_from_int(res, rng());
  } while (!is_prime_fermat(res));
}


