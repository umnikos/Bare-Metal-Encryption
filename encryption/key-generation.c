#include "prelude.h"

void generate_keys(ii* n, ii* e, ii* d) {
  mkii(temp1);
  mkii(temp2);
  mkii(p);
  mkii(q);
  generate_prime_fermat(5, p);
  printf("p: ");
  print_bignum(p);
  generate_prime_fermat(4, q);
  printf("q: ");
  print_bignum(q);
  bignum_mul(p, q, n);
  printf("N: ");
  print_bignum(n);
  // d = (k * phi(N) + 1) / e
  bignum_dec(p);
  bignum_dec(q);
  mkii(phi_n);
  bignum_mul(p,q,phi_n);

  bignum_from_int(e, 65535); // 65537
  printf("picking e...\n");
  do {
    bignum_inc(e);
    bignum_inc(e);
    gcd(e, phi_n, temp1);
    bignum_dec(temp1);
  } while (!bignum_is_zero(temp1));
  printf("e: ");
  print_bignum(e);

  mkii(k);
  bignum_init(k);

  // (ka+1)/b
  // (ka+ax+by)/b
  // (ka+ax)/b + y
  // a(x+k)/b + y
  // a(x+b-x)/b + y
  // a*b/b + y
  // k = b-x
  // TODO - CALCULATE K
  return;

  printf("k: ");
  print_bignum(k);

  bignum_mul(k, phi_n, temp1);
  bignum_div(temp1, e, d);
  printf("d: ");
  print_bignum(d);
}
