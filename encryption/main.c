#include "prelude.h"

void print_bignum(ii* num) {
  char buf[257];
  bignum_to_string(num, buf, 256);
  printf("%s\n", buf);
}

int main() {
  init_rng();
  mkii(n);
  mkii(e);
  mkii(d);
  generate_keys(n, e, d);

  mkii(message);
  bignum_from_int(message, 0x1337c0de);
  printf("message: ");
  print_bignum(message);
  mkii(encrypted);
  modular_exponentiation(message, e, n, encrypted);
  printf("encrypted: ");
  print_bignum(encrypted);
  mkii(decrypted);
  modular_exponentiation(encrypted, d, n, decrypted);
  printf("decrypted: ");
  print_bignum(decrypted);

}
