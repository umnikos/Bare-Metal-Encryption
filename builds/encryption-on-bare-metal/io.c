#include "prelude.h"

void write_out(const char* str);
void read_in(char* buf, i size);

void bn_to_str(ii num_in, char* buf) {
  inii(num);
  mkii(tmp);
  i j=0;
  while(!bignum_is_zero(num)) {
    i fourbits = num->array[0] & 0xF;
    
  }
}

void bn_from_str(char* buf, ii num) {
  // TODO
}
