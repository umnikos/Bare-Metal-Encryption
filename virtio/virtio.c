#include "../kernel/prelude.h"
#include "../kernel/idt.h"
#include "virtio.h"

extern u8* gimme_memory(u32 pages);
extern void virtio_handler_prelude();
extern void set_irq(u8 irq);
void disable_interrupts();
void enable_interrupts();

void i_to_str(u32 num, char* buf, u32 size);
void virtq_insert(struct virtio_device* virtio, u16 queue_num, char const* buf, u32 len, u16 flags);
u16 pci_read_config(u32 bus, u32 device, u32 func, u32 offset);
u16 pci_read_headertype(u32 bus, u32 device);
u16 pci_read_vendor(u32 bus, u32 device);
u16 pci_read_deviceid(u32 bus, u32 device);
u16 pci_read_subsystem(u32 bus, u32 device);
u8 pci_read_irq(u32 bus, u32 device);
u32 pci_read_bar(u32 bus, u32 device, u32 number);
void virtio_init(struct virtio_device* virtio);
void pci_find_virtio(struct virtio_device* virtio);
void virtio_negotiate(u32* supported_features);
void virtio_queues(struct virtio_device* virtio);
void virtio_handler();

volatile u8 driver_ok = 1;
volatile struct virtio_device* virtio_for_irq;

#define config_address  0x0CF8
#define config_data     0x0CFC
u16 pci_read_config(u32 bus, u32 device, u32 func, u32 offset) {
  // https://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
  u32 address = (u32)((bus << 16) | (device << 11) |
                     (func << 8) | (offset & 0xfc) | ((u32)0x80000000));
  out_dword(config_address, address);
  // the lowest bit of offset is ignored because we're outputting 16 bits and not 8
  u16 tmp = (u16)((in_dword(config_data) >> ((offset & 2) * 8)) & 0xffff);
  return tmp;
}

u16 pci_read_headertype(u32 bus, u32 device) {
  return pci_read_config(bus,device,0,0x0E) & 0x00FF;
}

u16 pci_read_vendor(u32 bus, u32 device) {
  return pci_read_config(bus,device,0,0);
}

u16 pci_read_deviceid(u32 bus, u32 device) {
  return pci_read_config(bus,device,0,2);
}

u16 pci_read_subsystem(u32 bus, u32 device) {
  return pci_read_config(bus,device,0,0x2E);
}

u8 pci_read_irq(u32 bus, u32 device) {
  return (u8)pci_read_config(bus,device,0,0x3C);
}

u32 pci_read_bar(u32 bus, u32 device, u32 number) {
  u16 bottom = pci_read_config(bus,device,0, 0x10 + number*4);
  u16 top = pci_read_config(bus,device,0, 0x12 + number*4);
  return ((u32)top<<16)+(u32)bottom;
}

void pci_find_virtio(struct virtio_device* result) {
  for(u32 bus=0; bus<256; bus++) {
    for(u32 device=0; device<32; device++) {
      u16 deviceid = pci_read_deviceid(bus, device);
      // there should be a subsystem check
      // but I don't know what the subsystem number of virtio-serial is
      // (it happens to be 3)
      // TODO - support for more than one virtio device
      if (deviceid >= 0x1000 &&
          deviceid <= 0x103F && // search only for transitional virtio devices
          pci_read_vendor(bus,device) == 0x1AF4) {
        if (pci_read_subsystem(bus,device) != 3) {
          crash("DEVICE ID IS NOT 3");
        }
        result->bus = bus;
        result->device = device;
        result->iobase = pci_read_bar(bus, device, 0) & 0xFFFC; // we only take the bottom word for some reason
        debug("found virtio device\n");
        return;
      }
    }
  }
  // if nothing has been found then there's nothing to do but hang
  crash("NO VIRTIO DEVICE FOUND");
}


#define VIRTIO_ACKNOWLEDGE 1
#define VIRTIO_DRIVER 2
#define VIRTIO_FEATURES_OK 8
#define VIRTIO_DRIVER_OK 4
#define VIRTIO_FAILED 128
#define VIRTIO_DEVICE_NEEDS_RESET 64

void virtio_init(struct virtio_device* res) {
  // http://www.dumais.io/index.php?article=aca38a9a2b065b24dfa1dee728062a12
  // iobase offsets are from virtio spec 4, section 4.1.4.8
  // http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.pdf

  /* from testing:
   * bar0 = c041
   * 1100 0000 0100 0001
   *
   * bar1 = 1000
   * 0001 0000 0000 0000
   */
  debug("start init\n");
  pci_find_virtio(res);
  virtio_for_irq = res;

  u8 irq = pci_read_irq(res->bus, res->device);
  debug("irq is: ");
  char debug_buf[65];
  i_to_str((u32)irq, debug_buf, 63);
  debug(debug_buf);
  debug("\n");
  if (irq < 8) {
    crash("IRQ IN FIRST RANGE!\n");
  } else if (irq >= 8 && irq < 16) {
    debug("irq in second range\n");
  } else {
    crash("IRQ IN UNKNOWN RANGE!\n");
  }
  set_irq(irq);

  u16 iobase = res->iobase;
  u8 status = VIRTIO_ACKNOWLEDGE;
  // FIXME - reset device before initialization (section 4.3.3.3)
  u8 status_in = in_byte(iobase+0x12);
  if (status_in & VIRTIO_DEVICE_NEEDS_RESET) {
    crash("THE DEVICE NEEDS A RESET!\n");
  }

  out_byte(iobase+0x12, status);
  status |= VIRTIO_DRIVER;
  out_byte(iobase+0x12, status);
  u32 supported_features = in_dword(iobase+0x00);
  virtio_negotiate(&supported_features);
  out_dword(iobase+0x04, supported_features);
  /* section 3.1.2 tells us to omit these two steps
  debug("before status\n");
  status |= VIRTIO_FEATURES_OK;
  out_byte(iobase+0x12, status);
  debug("after status\n");
  debug("before check\n");
  if ((in_byte(iobase+0x12) & VIRTIO_FEATURES_OK) == 0) {
    // the negotiations have failed!
    status |= VIRTIO_FAILED;
    out_byte(iobase+0x12, status);
    crash("FAILED NEGOTIATIONS");
  }
  debug("after check\n");
  */
  virtio_queues(res);
  debug("after queues\n");
  status_in = in_byte(iobase+0x12);
  if (status_in & VIRTIO_DEVICE_NEEDS_RESET) {
    crash("THE DEVICE NEEDS A RESET!\n");
  }

  status |= VIRTIO_DRIVER_OK;
  debug("after status variable\n");
  out_byte(iobase+0x12, status);
  debug("after status\n");

  driver_ok = 1;
  debug("end init\n");
}

void virtio_negotiate(u32* supported_features) {
  /* from testing:
   * supported_features = 79000006
   * 0111 1001 0000 0000 0000 0000 0000 0110
   */
  debug("start negotiation\n");
  *supported_features &= 0x00000000;
  debug("end negotiation\n");
}

void virtio_queues(struct virtio_device* virtio) {
  debug("start virtio_queues\n");
  u16 iobase = virtio->iobase;
  // section 5.3.2 tells us what the two queues are
  for (u16 q_addr=0; q_addr<2; q_addr++) {

    // sections 2.4 and onward describes the queue layout

    // get queue size
    out_word(iobase+0x0E,q_addr);
    u16 q_size = in_word(iobase+0x0C);
    if (q_size == 0) {
      crash("QSIZE IS 0");
    }

    // allocate queue
    // formula given by section 2.4.2
    u32 sizeofBuffers = (sizeof(struct virtq_desc)*q_size);
    u32 sizeofQueueAvailable = (sizeof(u16)*(3+q_size));
    u32 sizeofQueueUsed = (sizeof(u16)*3 + sizeof(struct virtq_used_elem)*q_size);
#define page_count(bytes) ((bytes+0x0FFF)>>12)
    u32 firstPageCount = page_count(sizeofBuffers + sizeofQueueAvailable);
    u32 secondPageCount = page_count(sizeofQueueUsed);
    u32 queuePageCount = firstPageCount+secondPageCount;
    u8* buf = gimme_memory(queuePageCount);
    u32 bufPage = (uptr)buf >> 12;

    struct virtq* vq = &virtio->queues[q_addr];
    vq->qsize = q_size;
    vq->desc = (struct virtq_desc*)buf;
    vq->avail = (struct virtq_avail*)(buf+sizeofBuffers);
    vq->used = (struct virtq_used*)(buf+(firstPageCount<<12));

    vq->avail->idx = 0;
    vq->avail->flags = 0; // 1 if we don't want interrupts, 0 otherwise
    vq->used->idx = 0;
    vq->used->flags = 0;

    if (q_addr == 0) {
      static char input_buff[1025];
      virtq_insert(virtio, 0, input_buff, 1023, 2);
    }

    out_word(iobase+0x0E,q_addr);
    out_dword(iobase+0x08,bufPage);
    debug("virtqueue created\n");
  }

  debug("end virtio_queues\n");
}

void virtq_insert(struct virtio_device* virtio, u16 queue_num, char const* buf, u32 len, u16 flags) {
  u16 iobase = virtio->iobase;
  struct virtq* queue = &virtio->queues[queue_num];
  // find next free buffer slot
  // TODO - actual searching for empty descriptors
  static u16 buf_index = 0;
  buf_index++;

  debug("virtq_insert started\n");

  // set buffer slot's address to message string
  queue->desc[buf_index].addr = (uptr)buf;
  debug("virtq_insert address\n");
  queue->desc[buf_index].len = len;
  debug("virtq_insert length\n");
  queue->desc[buf_index].flags = flags;
  debug("virtq_insert flags\n");

  // add it in the available ring
  if (queue->qsize == 0) {
    crash("QSIZE IS ZERO");
    return;
  }
  u16 index = (queue->avail->idx) % (queue->qsize);
  debug("virtq_insert calculated index\n");
  queue->avail->ring[index] = buf_index;
  debug("virtq_insert added to ring\n");
  mem_barrier; // section 3.2.1.3.1
  queue->avail->idx++;
  debug("virtq_insert incremented index\n");
  mem_barrier; // section 3.2.1.4.1

  // notify the device that there's been a change
  if (driver_ok) {
    debug("virtq_insert about to notify\n");
    out_word(iobase+0x10,queue_num);
    debug("virtq_insert notified\n");
  }

}

extern struct idt_entry idt[];

extern void virtio_handler_prelude();

void set_irq(u8 irq) {
  disable_interrupts();
  idt[irq+0x20].offset_1 = (u16)(uptr)virtio_handler_prelude;
  idt[irq+0x20].offset_2 = (u16)((uptr)virtio_handler_prelude >> 16);
  enable_interrupts();
}

void virtio_handler() {
  debug("IRQ\n");

  // notify device it's all good now
  // in_byte(virtio_for_irq->iobase+0x12);
  nothing();

  disable_interrupts();
}

