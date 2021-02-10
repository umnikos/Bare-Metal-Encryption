#include "../../encryption/prelude.h"
#include "../../serial/serial.h"

// --- IO ---

void write_out(const char* str) {
  serial_send(str);
}

void read_in(char* buf, i size) {
  serial_receive(buf, (u32)size);
}

void flush_out() {
  return;
}
