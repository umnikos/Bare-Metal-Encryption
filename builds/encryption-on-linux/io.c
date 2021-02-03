#include <stdio.h>
#include "../../encryption/prelude.h"

void write_out(const char* str) {
  printf("%s", str);
}

void flush_out() {
  fflush(stdout);
}

void read_in(char* buf, i size) {
  // TODO
}

void i_to_str(i num, char* buf, i size) {
  sprintf(buf, "%x", num);
}

void bn_to_str(ii* num_in, char* buf, i size) {
  bignum_to_string(num_in, buf, size);
}

void bn_from_str(char* buf, ii* num, i size) {
  bignum_from_string(num, buf, size);
}
