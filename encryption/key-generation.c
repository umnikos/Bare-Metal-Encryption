#include "prelude.h"

void generate_keys(ii* n, ii* e, ii* d) {
  mkii(temp1);
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
  si x; // to be used while calculating k
  do {
    bignum_inc(e);
    bignum_inc(e);
    x = gcd_extended(phi_n, e, temp1);
    bignum_dec(temp1);
  } while (!bignum_is_zero(temp1));
  printf("e: ");
  print_bignum(e);

  // (ka+1)/b
  // (ka+ax+by)/b
  // (ka+ax)/b + y
  // a(x+k)/b + y
  // a(x+b-x)/b + y
  // ab/b + y
  // ==> k = b-x
  mkii(k);
  printf("x: %x\n", x);
  if (x>=0) {
    // x is positive
    bignum_from_int(temp1, x);
    bignum_sub(e, temp1, k);
  } else {
    // x is negative
    printf("-x: %x\n", -x);
    bignum_from_int(temp1, -x);
    bignum_add(e, temp1, k);
  }

  printf("k: ");
  print_bignum(k);

  bignum_mul(k, phi_n, temp1);
  bignum_inc(temp1);
  // check if k is correct
  bignum_mod(temp1, e, d);
  if (bignum_is_zero(d)) {
    printf("k is correct.\n");
  } else {
    printf("INCORRECT K!!!\n");
  }
  bignum_div(temp1, e, d);
  printf("d: ");
  print_bignum(d);
}
