#include "prelude.h"

void print_bignum(ii* num) {
  char buf[257];
  bn_to_str(num, buf, 256);
  write_out(buf);
  write_out("\n");
}
