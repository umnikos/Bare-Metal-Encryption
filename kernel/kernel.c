#include "prelude.h"
#include "virtio.h"

void hello_world(struct virtio_device virtio);

void kernel_main() {
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

  // set buffer slot's address to message string
  output_queue->desc[buf_index].addr = ((u64)(u32)msg) & 0xFFFFFFFF;
  output_queue->desc[buf_index].len = msglen;
  output_queue->desc[buf_index].flags = 0;

  // add it in the available ring
  u16 index = output_queue->avail->idx % output_queue->qsize;
  output_queue->avail->ring[index] = buf_index;
  mem_barrier; // section 3.2.1.3.1
  output_queue->avail->idx++;
  mem_barrier; // section 3.2.1.4.1

  // notify the device that there's been a change
  out_word(iobase+0x10,1);

  // must not return or the stack will get destroyed
  while (1);
  crash(0x43434343);
}

