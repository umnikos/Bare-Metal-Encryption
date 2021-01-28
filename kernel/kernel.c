#include "prelude.h"
#include "virtio.h"
void terminal_initialize(void);

void hello_world(struct virtio_device virtio);

void kernel_main() {
  /* Initialize terminal interface */
  terminal_initialize();
  /* Newline support is left as an exercise. */
  debug("-DEBUG-\n");

  struct virtio_device virtio = virtio_init();
  hello_world(virtio);
  crash(0x42424242);
}

void hello_world(struct virtio_device virtio) {
  const u32 msglen = 20;
  char msg[20] = "Hello, World!\n";

  u16 iobase = virtio.iobase;
  struct virtq* output_queue = &virtio.queues[1];
  // find next free buffer slot
  u16 buf_index = 10; // TODO - actual searching

  debug("hello world started\n");

  // set buffer slot's address to message string
  output_queue->desc[buf_index].addr = ((u64)(u32)msg) & 0xFFFFFFFF;
  debug("hello world address\n");
  output_queue->desc[buf_index].len = msglen;
  debug("hello world length\n");
  output_queue->desc[buf_index].flags = 0;
  debug("hello world flags\n");

  // add it in the available ring
  if (output_queue->qsize == 0) {
    debug("ZERO, BAKAAA!!!\n");
  }
  u16 index = (output_queue->avail->idx) % (output_queue->qsize);
  debug("hello world calculated index\n");
  output_queue->avail->ring[index] = buf_index;
  debug("hello world added to ring\n");
  mem_barrier; // section 3.2.1.3.1
  output_queue->avail->idx++;
  debug("hello world incremented index\n");
  mem_barrier; // section 3.2.1.4.1

  // notify the device that there's been a change
  debug("hello world about to notify\n");
  out_word(iobase+0x10,1);
  debug("hello world notified\n");

  // must not return or the stack will get destroyed
  while (1);
  crash(0x43434343);
}

