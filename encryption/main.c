#include "prelude.h"

void exponentiation(ii* b, ii* e, ii* res);

int main() {
  ii a, b;
  bignum_from_int(&a, 13);
  bignum_from_int(&b, 19);
  ii c;
  exponentiation(&a, &b, &c);
  char buf[256];
  bignum_to_string(&c, buf, sizeof(buf));
  printf("%s\n", buf);
}

