#include "prelude.h"
#include "virtio.h"
void terminal_initialize();
void virtq_insert(struct virtio_device* virtio, u32 queue_num, char const* buf, u32 len, u32 flags);

void hello_world(struct virtio_device *virtio);

void kernel_main() {
  /* Initialize terminal interface */
  terminal_initialize();
  /* Newline support is left as an exercise. */
  debug("-DEBUG-\n");

  struct virtio_device virtio;
  virtio_init(&virtio);
  hello_world(&virtio);
  crash(0x42424242);
}

void hello_world(struct virtio_device* virtio) {
  const u32 msglen = 16;
  static const char msg[] = "Hello, World!\n\0\0\0\0\0";

  virtq_insert(virtio, 1, msg, msglen, 0);

  while (1);
}

