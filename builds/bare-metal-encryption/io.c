#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../virtio/virtio.h"

// --- IO ---

void write_out(const char* str) {
  i len = 0;
  while (str[len] != '\0') {
    len++;
  }
  virtq_insert(&virtio, 1, str, len, 0);
}

void read_in(char* buf, i size) {
  // TODO
}

void flush_out() {
  return;
}
