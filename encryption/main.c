#include "prelude.h"

void print_bignum(ii* num) {
  char buf[257];
  bn_to_str(num, buf, 256);
  write_out(buf);
  write_out("\n");
}

int main() {
  init_rng();
  mkii(n);
  mkii(e);
  mkii(d);
  generate_keys(n, e, d);

  mkii(message);
  bignum_from_int(message, 0x1337c0de);
  write_out("message: ");
  print_bignum(message);
  mkii(encrypted);
  modular_exponentiation(message, e, n, encrypted);
  write_out("encrypted: ");
  print_bignum(encrypted);
  mkii(decrypted);
  modular_exponentiation(encrypted, d, n, decrypted);
  write_out("decrypted: ");
  print_bignum(decrypted);

}
