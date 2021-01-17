#include "prelude.h"

void print_bignum(ii* num) {
  char buf[257];
  bignum_to_string(num, buf, 256);
  printf("%s\n", buf);
}

int main() {
  init_rng();
  mkii(p);
  while (true) {
    generate_prime_fermat(p);
    printf("prime: ");
    print_bignum(p);
  }
}

