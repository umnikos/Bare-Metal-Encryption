#pragma once

#include "virtio_queue.h"

struct virtio_device {
  u32 bus;
  u32 device;
  u16 iobase;
  struct virtq queues[2];
} __attribute__((packed));

void virtio_init(struct virtio_device* virtio);
