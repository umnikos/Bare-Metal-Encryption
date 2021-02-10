#include "../../encryption/prelude.h"
#include "../../kernel/prelude.h"

void write_out(const char* str) {
  debug(str);
}

void flush_out() {
  return;
}

void read_in(char* buf, i size) {
  // TODO
}

const char hex_symbols[16] = "0123456789ABCDEF";
void i_to_str(i num, char* buf, i size) {
  size--;
  i j = 0;
  do {
    u8 four_bits = num & 0xF;
    buf[j] = hex_symbols[four_bits];
    j++;
    num = num >> 4;
  } while (num && j<size);
  buf[j] = '\0';
  // the number is in reverse so we have to... reverse it
  j--;
  i k = 0;
  while (j > k) {
    char tmp = buf[j];
    buf[j] = buf[k];
    buf[k] = tmp;
    k++;
    j--;
  }
}

void bn_to_str(ii* num_in, char* buf, i size) {
  inii(num);
  mkii(tmp_num);
  size--;
  i j = 0;
  do {
    u8 four_bits = num->array[0] & 0xF;
    buf[j] = hex_symbols[four_bits];
    j++;
    bignum_rshift(num, tmp_num, 4);
    bignum_assign(num, tmp_num);
  } while (!bignum_is_zero(num) && j<size);
  buf[j] = '\0';
  // the number is in reverse so we have to... reverse it
  j--;
  i k = 0;
  while (j > k) {
    char tmp = buf[j];
    buf[j] = buf[k];
    buf[k] = tmp;
    k++;
    j--;
  }
}

void bn_from_str(char* buf, ii* num, i size) {
  // TODO
}

i get_time() {
  // https://wiki.osdev.org/CMOS#The_Real-Time_Clock
  // FIXME - make it not just return seconds, lol
  out_byte(0x70, 0x80 | 0x00);
  fibonacci(10);
  i seconds = in_byte(0x71);
  return (seconds << 17) + 17;
}
