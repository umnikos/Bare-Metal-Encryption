#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../virtio/virtio.h"

// --- IO ---

void virtq_insert(struct virtio_device* virtio, u32 queue_num, char const* buf, u32 len, u16 flags);

void write_out(const char* str) {
  i len = 0;
  while (str[len] != '\0') {
    len++;
  }
  virtq_insert(&virtio, 1, str, len, 0);
}

void flush_out() {
  return;
}
