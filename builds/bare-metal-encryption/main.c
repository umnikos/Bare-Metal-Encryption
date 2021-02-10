#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../virtio/virtio.h"

void terminal_initialize();

static struct virtio_device virtio;

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  virtio_init(&virtio);

  rsa_main();
}

#include "io.c"
#include "misc.c"
