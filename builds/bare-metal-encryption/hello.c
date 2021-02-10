#include "../../kernel/prelude.h"
#include "../../virtio/virtio.h"

void terminal_initialize();

void hello_world(struct virtio_device *virtio);

struct virtio_device virtio;

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  virtio_init(&virtio);
  hello_world(&virtio);
  crash("MAIN FINISHED");
}

void nothing(void* whatever);

void hello_world(struct virtio_device* virtio) {
  debug("got to hello world!\n");

  const u32 msglen = 16;
  static const char msg[] = "Hello, World!\n\0\0\0\0\0";

  virtq_insert(virtio, 1, msg, msglen, 0);

  debug("end of hello world\n");
  while (1);
}

#include "io.c"
#include "misc.c"
