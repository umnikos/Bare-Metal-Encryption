#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../virtio/virtio.h"

void terminal_initialize();

static struct virtio_device virtio;

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  virtio_init(&virtio);

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

void virtq_insert(struct virtio_device* virtio, u32 queue_num, char const* buf, u32 len, u8 flags);

void write_out(const char* str) {
  i len = 0;
  while (str[len] != '\0') {
    len++;
  }
  virtq_insert(&virtio, 1, str, len, 0);
}
