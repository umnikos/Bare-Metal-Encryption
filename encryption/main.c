#include "prelude.h"

void exponentiation(ii* b, ii* e, ii* res);
void modular_exponentiation(ii* b, ii* e, ii* m, ii* res);

int main() {
  ii a, b, m;
  bignum_from_int(&a, 13);
  bignum_from_int(&b, 19);
  bignum_from_int(&m, 27);
  ii c;
  //exponentiation(&a, &b, &c);
  modular_exponentiation(&a, &b, &m, &c);
  char buf[256];
  bignum_to_string(&c, buf, sizeof(buf));
  printf("%s\n", buf);
}

