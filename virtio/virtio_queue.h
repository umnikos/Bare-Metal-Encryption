#pragma once

// taken from appendix A in the specification: http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.pdf
#include <stdint.h>

// this marks a buffer as continuing via the next field
#define VIRTQ_DESC_F_NEXT       1
// this marks a buffer as device write-only (otherwise read-only)
#define VIRTQ_DESC_F_WRITE      2

// Virtqueue descriptors: 16 bytes.
// These can chain together via "next"
struct virtq_desc {
  uint64_t addr; // physical address of the data
  uint32_t len; // data length
  uint16_t flags; // flags described above
  uint16_t next; // descriptor chaining
};

// avaiable header
// only written by driver and read by the device
struct virtq_avail {
  uint16_t flags; // flags
  uint16_t idx; // index of next free slot in ring
  uint16_t ring[]; // list of frames. there is no virtq_avail_elem to hold so it holds heads to descriptor chains
};

struct virtq_used_elem {
  uint32_t id; // index of start of used descriptor chain. uint32_t for padding reasons
  uint32_t len; // total number of bytes written to buffer
};

// used header
// only written by device and read by the driver
struct virtq_used {
  uint16_t flags; // flags
  uint16_t idx; // index of next free slot in ring
  struct virtq_used_elem ring[]; // list of frames
};

struct virtq {
  struct virtq_desc *desc; // list of frames holding pointers to memory addresses with data
  struct virtq_avail *avail; // list of free frames for sending new data
  struct virtq_used *used; // list of used frames
  uint16_t qsize;
};

