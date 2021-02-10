#pragma once

#include "virtio_queue.h"

struct virtio_device {
  u32 bus;
  u32 device;
  u16 iobase;
  struct virtq queues[2];
};

void virtio_init(struct virtio_device* virtio);
void virtq_insert(struct virtio_device* virtio, u16 queue_num, char const* buf, u32 len, u16 flags);
