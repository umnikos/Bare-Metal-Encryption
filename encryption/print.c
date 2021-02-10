#include "prelude.h"

void print_bignum(ii* num) {
  char buf[257];
  bn_to_str(num, buf, 256);
  write_out(buf);
  write_out("\n");
}

void print_num(i num) {
  char buf[257];
  i_to_str(num, buf, 256);
  write_out(buf);
  write_out("\n");
}
