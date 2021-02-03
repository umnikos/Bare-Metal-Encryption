#include "../../kernel/prelude.h"
#include "../../virtio/virtio.h"

void terminal_initialize();
void virtq_insert(struct virtio_device* virtio, u32 queue_num, char const* buf, u32 len, u32 flags);

void hello_world(struct virtio_device *virtio);

struct virtio_device virtio;

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  virtio_init(&virtio);
  hello_world(&virtio);
  crash(0x42424242);
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

