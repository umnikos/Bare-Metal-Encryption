#include "prelude.h"

void generate_keys(ii* n, ii* e, ii* d) {
  char buf[64];
  mkii(temp1);
  mkii(p);
  mkii(q);
  generate_prime_fermat(5, p);
  write_out("p: ");
  print_bignum(p);
  generate_prime_fermat(4, q);
  write_out("q: ");
  print_bignum(q);
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
  write_out("x: ");
  i_to_str((i)x, buf, 64);
  write_out(buf);
  write_out("\n");
  if (x>=0) {
    // x is positive
    bignum_from_int(temp1, (li)x);
    bignum_sub(e, temp1, k);
  } else {
    // x is negative
    write_out("-x: ");
    i_to_str((i)-x, buf, 64);
    write_out(buf);
    write_out("\n");
    bignum_from_int(temp1, (li)-x);
    bignum_add(e, temp1, k);
  }

  write_out("k: ");
  print_bignum(k);

  bignum_mul(k, phi_n, temp1);
  bignum_inc(temp1);
  // check if k is correct
  bignum_mod(temp1, e, d);
  if (bignum_is_zero(d)) {
    write_out("k is correct.\n");
  } else {
    write_out("INCORRECT K!!!\n");
  }
  bignum_div(temp1, e, d);
  write_out("d: ");
  print_bignum(d);
}
