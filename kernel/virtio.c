#include "prelude.h"
#include "virtio.h"

extern u8* gimme_memory(u32 pages);
extern void virtio_handler_prelude();
extern void set_irq(u8 irq);

struct virtio_device res;

void serial_print(char* str); // TODO
u16 pci_read_config(u32 bus, u32 device, u32 func, u32 offset);
u16 pci_read_headertype(u32 bus, u32 device);
u16 pci_read_vendor(u32 bus, u32 device);
u16 pci_read_deviceid(u32 bus, u32 device);
u16 pci_read_subsystem(u32 bus, u32 device);
u16 pci_read_irq(u32 bus, u32 device);
u32 pci_read_bar(u32 bus, u32 device, u32 number);
struct virtio_device virtio_init();
struct virtio_device pci_find_virtio();
void virtio_negotiate(u32* supported_features);
void virtio_queues(struct virtio_device* virtio);
void virtio_handler();

volatile u8 virtqueue_setup = 0;

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

u16 pci_read_irq(u32 bus, u32 device) {
  return pci_read_config(bus,device,0,0x3C) & 0x00FF;
}

u32 pci_read_bar(u32 bus, u32 device, u32 number) {
  u16 bottom = pci_read_config(bus,device,0, 0x10 + number*4);
  u16 top = pci_read_config(bus,device,0, 0x12 + number*4);
  return (top<<16)+bottom;
}

struct virtio_device pci_find_virtio() {
  struct virtio_device result;
  for(u32 bus=0; bus<256; bus++) {
    for(u32 device=0; device<32; device++) {
      u16 deviceid = pci_read_deviceid(bus, device);
      // there should be a subsystem check
      // but I don't know what the subsystem number of virtio-serial is
      // (it happens to be 3)
      if (deviceid >= 0x1000 &&
          deviceid <= 0x103F && // search only for transitional virtio devices
          pci_read_vendor(bus,device) == 0x1AF4) {
        result.bus = bus;
        result.device = device;
        result.iobase = pci_read_bar(bus, device, 0) & 0xFFFC; // we only take the bottom word for some reason
        return result;
      }
    }
  }
  // if nothing has been found then there's nothing to do but hang
  halt();
  while (1); // gets rid of compiler warning
}


#define VIRTIO_ACKNOWLEDGE 1
#define VIRTIO_DRIVER 2
#define VIRTIO_FEATURES_OK 8
#define VIRTIO_DRIVER_OK 4
#define VIRTIO_FAILED 128
#define VIRTIO_DEVICE_NEEDS_RESET 64

struct virtio_device virtio_init(struct virtio_device res) {
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
  res = pci_find_virtio();
  u8 irq = pci_read_irq(res.bus, res.device);
  set_irq(0x20+irq);
  u16 iobase = res.iobase;
  u8 status = VIRTIO_ACKNOWLEDGE;
  out_byte(iobase+0x12, status);
  status |= VIRTIO_DRIVER;
  out_byte(iobase+0x12, status);
  u32 supported_features = in_dword(iobase+0x00);
  virtio_negotiate(&supported_features);
  out_dword(iobase+0x04, supported_features);
  /* section 3.1.2 tells us to omit these two steps
  status |= VIRTIO_FEATURES_OK;
  out_byte(iobase+0x12, status);
  if ((in_byte(iobase+0x12) & VIRTIO_FEATURES_OK) == 0) {
    // the negotiations have failed!
    status |= VIRTIO_FAILED;
    out_byte(iobase+0x12, status);
    crash(0xBABADEAD);
  }
  */
  virtio_queues(&res);
  status |= VIRTIO_DRIVER_OK;
  out_byte(iobase+0x12, status);
  virtqueue_setup = 1;
  return res;
}

void virtio_negotiate(u32* supported_features) {
  /* from testing:
   * supported_features = 79000006
   * 0111 1001 0000 0000 0000 0000 0000 0110
   */
  *supported_features &= 0xFF000000; // TODO - figure out what the top 8 bits do
}

void virtio_queues(struct virtio_device* virtio) {
  u16 iobase = virtio->iobase;
  // section 5.3.2 tells us what the two queues are
  // only queue #1 is needed for output
  u32 q_addr = 1;

  // sections 2.4 and onward describes the queue layout

  // get queue size
  out_word(iobase+0x0E,1);
  u16 q_size = in_word(iobase+0x0C);
  if (q_size == 0) {
    crash(0x01010101);
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
  u32 bufPage = (u32)buf >> 12;

  struct virtq* vq = &virtio->queues[1];
  vq->qsize = q_size;
  vq->desc = (struct virtq_desc*)buf;
  vq->avail = (struct virtq_avail*)(buf+sizeofBuffers);
  vq->used = (struct virtq_used*)(buf+(firstPageCount<<12));

  vq->avail->idx = 0;
  vq->avail->flags = 0;
  vq->used->idx = 0;
  vq->used->flags = 0;

  out_word(iobase+0x0E,q_addr);
  out_dword(iobase+0x08,bufPage);
}

void virtio_handler() {
  in_byte(res.iobase+0x12);
  if (virtqueue_setup) {
    crash(0xBABABABA);
  }
  nothing();
}

