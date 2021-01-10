#include "prelude.h"

int main() {
  ii a, b;
  bignum_from_int(&a, 2);
  bignum_from_int(&b, 3);
  ii c;
  bignum_init(&c);
  bignum_add(&a, &b, &c);
  char buf[256];
  bignum_to_string(&c, buf, sizeof(buf));
  printf("%s\n", buf);
}

