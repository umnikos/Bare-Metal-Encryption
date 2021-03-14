#include "prelude.h"

#define DISPLAY_KEY() 0

void generate_keys(ii* n, ii* e, ii* d) {
  mkii(temp1);
  mkii(p);
  mkii(q);
  generate_prime_fermat(5, p);
#if DISPLAY_KEY()
  write_out("p: ");
  print_bignum(p);
#else
  write_out("p generated\n");
#endif
  generate_prime_fermat(4, q);
#if DISPLAY_KEY()
  write_out("q: ");
  print_bignum(q);
#else
  write_out("q generated\n");
#endif
  bignum_mul(p, q, n);
  write_out("N: ");
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
  write_out("e: ");
  print_bignum(e);

  // (ka+1)/b
  // (ka+ax+by)/b
  // (ka+ax)/b + y
  // a(x+k)/b + y
  // a(x+b-x)/b + y
  // ab/b + y
  // ==> k = b-x
  mkii(k);
#if DISPLAY_KEY()
  write_out("x: ");
  print_num((i)x);
#endif
  if (x>=0) {
    // x is positive
    bignum_from_int(temp1, (li)x);
    bignum_sub(e, temp1, k);
  } else {
    // x is negative
#if DISPLAY_KEY()
    write_out("-x: ");
    print_num((i)-x);
#endif
    bignum_from_int(temp1, (li)-x);
    bignum_add(e, temp1, k);
  }

#if DISPLAY_KEY()
  write_out("k: ");
  print_bignum(k);
#endif

  bignum_mul(k, phi_n, temp1);
  bignum_inc(temp1);
  // check if k is correct
  bignum_mod(temp1, e, d);
  if (bignum_is_zero(d)) {
    // write_out("k is correct.\n");
  } else {
    write_out("INCORRECT K!!!\n");

    write_out("key generation failed. try again?\n");
    return;
  }
  bignum_div(temp1, e, d);
#if DISPLAY_KEY()
  write_out("d: ");
  print_bignum(d);
#endif
  write_out("key generation done.\n");
}
