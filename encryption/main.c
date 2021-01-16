#include "prelude.h"

int main() {
  ii a, b, m;
  bignum_from_int(&a, 45);
  bignum_from_int(&b, 9);
  bignum_from_int(&m, 27);
  ii c;
  //exponentiation(&a, &b, &c);
  //modular_exponentiation(&a, &b, &m, &c);
  //gcd(&a, &b, &c);
  lcm(&a, &b, &c);
  char buf[256];
  bignum_to_string(&c, buf, sizeof(buf));
  printf("%s\n", buf);
}

