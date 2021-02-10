#include "prelude.h"

void rsa_main() {
  char in[1025];
  char out[1025];
  mkii(temp1);
  mkii(temp2);
  mkii(n);
  mkii(e);
  mkii(d);
  bignum_init(n);

  while (true) {
    write_out("\nRSA ENCRYPTION CONSOLE\n");
    write_out("0 - exit\n");
    write_out("1 - generate key pair\n");
    // FIXME - prevent options 2 and 3 from actually being selected in the switch below
    if (!bignum_is_zero(n)) {
      write_out("2 - encrypt message\n");
      write_out("3 - decrypt message\n");
    }
    write_out("> ");
    read_in(in, 1);
    switch (in[0]) {
      case '0':
        return;
        break;
      case '1':
        init_rng();
        generate_keys(n,e,d);
        break;
      case '2':
        // TODO - ADD RANDOM PADDING FOR SECURITY
        write_out("message: ");
        read_in(in, 1024);
        char* s = in;
        mkii(m);
        i count=0;
        while (true) {
          bignum_init(m);
          count = 0;
          while (*s != '\0' && count < 128) {
            bignum_lshift(m, temp1, 8);
            bignum_from_int(temp2, (li)(*s));
            bignum_add(temp1, temp2, m);
            count++;
            s++;
          }
          if (count == 0) break;
          modular_exponentiation(m, e, n, temp1);
          bn_to_str(temp1, out, 1024);
          write_out(out);
          write_out("|");
        }
        write_out("\n");
        break;
      case '3':
        break;
      default:
        write_out("unknown option.\n");
        break;
    }
  }
}

void rsa_demo() {
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
