#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"

void terminal_initialize();

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  init_rng();
  mkii(n);
  mkii(e);
  mkii(d);
  generate_keys(n, e, d);

  mkii(message);
  bignum_from_int(message, 0x0c0def00d);
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

#include "io-vga.c"
