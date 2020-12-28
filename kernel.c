#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "virtio_queue.h"

extern void crash(uint32_t error_code);
extern void out_byte(uint16_t port, uint8_t data);
extern void out_word(uint16_t port, uint16_t data);
extern void out_dword(uint16_t port, uint32_t data);
extern uint8_t in_byte(uint16_t port);
extern uint16_t in_word(uint16_t port);
extern uint32_t in_dword(uint16_t port);
extern void halt();
extern void waiting();

struct search_result {
  uint32_t bus;
  uint32_t device;
  uint16_t iobase;
};
void serial_print(char* str); // TODO
uint16_t pci_read_config(uint32_t bus, uint32_t device, uint32_t func, uint32_t offset);
uint16_t pci_read_headertype(uint32_t bus, uint32_t device);
uint16_t pci_read_vendor(uint32_t bus, uint32_t device);
uint16_t pci_read_deviceid(uint32_t bus, uint32_t device);
uint16_t pci_read_subsystem(uint32_t bus, uint32_t device);
uint16_t pci_read_irq(uint32_t bus, uint32_t device);
uint32_t pci_read_bar(uint32_t bus, uint32_t device, uint32_t number);
void virtio_init(struct search_result res);
struct search_result pci_find_virtio();
void virtio_negotiate(uint32_t* supported_features);
void virtio_queues(uint16_t iobase);
uint8_t* gimme_memory(uint32_t pages);
void hello_world(struct search_result);

#define mem_barrier asm volatile ("" : : : "memory")

void kernel_main() {
  struct search_result virtio = pci_find_virtio();
  virtio_init(virtio);
  hello_world(virtio);
  crash(0x42424242);
}

#define config_address  0x0CF8
#define config_data     0x0CFC
uint16_t pci_read_config(uint32_t bus, uint32_t device, uint32_t func, uint32_t offset) {
  // https://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
  uint32_t address = (uint32_t)((bus << 16) | (device << 11) |
                     (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
  out_dword(config_address, address);
  // the lowest bit of offset is ignored because we're outputting 16 bits and not 8
  uint16_t tmp = (uint16_t)((in_dword(config_data) >> ((offset & 2) * 8)) & 0xffff);
  return tmp;
}

uint16_t pci_read_headertype(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0x0E) & 0x00FF;
}

uint16_t pci_read_vendor(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0);
}

uint16_t pci_read_deviceid(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,2);
}

uint16_t pci_read_subsystem(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0x2E);
}

uint16_t pci_read_irq(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0x3C) & 0x00FF;
}

uint32_t pci_read_bar(uint32_t bus, uint32_t device, uint32_t number) {
  uint16_t bottom = pci_read_config(bus,device,0, 0x10 + number*4);
  uint16_t top = pci_read_config(bus,device,0, 0x12 + number*4);
  return (top<<16)+bottom;
}

struct search_result pci_find_virtio() {
  struct search_result result;
  for(uint32_t bus=0; bus<256; bus++) {
    for(uint32_t device=0; device<32; device++) {
      uint16_t deviceid = pci_read_deviceid(bus, device);
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
}


#define VIRTIO_ACKNOWLEDGE 1
#define VIRTIO_DRIVER 2
#define VIRTIO_FEATURES_OK 8
#define VIRTIO_DRIVER_OK 4
#define VIRTIO_FAILED 128
#define VIRTIO_DEVICE_NEEDS_RESET 64

void virtio_init(struct search_result res) {
  // http://www.dumais.io/index.php?article=aca38a9a2b065b24dfa1dee728062a12
  // iobase offsets are from virtio spec 4, section 4.1.4.8
  // http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.pdf

  //uint16_t irq = pci_read_irq(bus, device);
  //setup_irq(irq); // I'm pretty sure this is only necessary for input

  /* from testing:
   * bar0 = c041
   * 1100 0000 0100 0001
   *
   * bar1 = 1000
   * 0001 0000 0000 0000
   */
  uint16_t iobase = res.iobase;
  uint8_t status = VIRTIO_ACKNOWLEDGE;
  out_byte(iobase+0x12, status);
  status |= VIRTIO_DRIVER;
  out_byte(iobase+0x12, status);
  uint32_t supported_features = in_dword(iobase+0x00);
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
  virtio_queues(iobase);
  status |= VIRTIO_DRIVER_OK;
  out_byte(iobase+0x12, status);
}

void virtio_negotiate(uint32_t* supported_features) {
  /* from testing:
   * supported_features = 79000006
   * 0111 1001 0000 0000 0000 0000 0000 0110
   */
  *supported_features &= 0xFF000000; // TODO - figure out what the top 8 bits do
}

//struct virtq input_queue;
struct virtq output_queue;

void virtio_queues(uint16_t iobase) {
  // section 5.3.2 tells us what the two queues are
  // only queue #1 is needed for output
  uint32_t q_addr = 1;

  // sections 2.4 and onward describes the queue layout

  // get queue size
  out_word(iobase+0x0E,1);
  uint16_t q_size = in_word(iobase+0x0C);
  if (q_size == 0) {
    crash(0x01010101);
  }

  // allocate queue
  // formula given by section 2.4.2
  uint32_t sizeofBuffers = (sizeof(struct virtq_desc)*q_size);
  uint32_t sizeofQueueAvailable = (sizeof(uint16_t)*(3+q_size));
  uint32_t sizeofQueueUsed = (sizeof(uint16_t)*3 + sizeof(struct virtq_used_elem)*q_size);
#define page_count(bytes) ((bytes+0x0FFF)>>12)
  uint32_t firstPageCount = page_count(sizeofBuffers + sizeofQueueAvailable);
  uint32_t secondPageCount = page_count(sizeofQueueUsed);
  uint32_t queuePageCount = firstPageCount+secondPageCount;
  uint8_t* buf = gimme_memory(queuePageCount);
  uint32_t bufPage = (uint32_t)buf >> 12;

  struct virtq* vq = &output_queue;
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

void hello_world(struct search_result virtio) {
  const uint32_t msglen = 20;
  char msg[20] = "Hello, World!\n";

  uint16_t iobase = virtio.iobase;
  // find next free buffer slot
  uint16_t buf_index = 10; // TODO - actual searching

  // set buffer slot's address to message string
  output_queue.desc[buf_index].addr = ((uint64_t)(uint32_t)msg) & 0xFFFFFFFF;
  output_queue.desc[buf_index].len = msglen;
  output_queue.desc[buf_index].flags = 0;

  // add it in the available ring
  uint16_t index = output_queue.avail->idx % output_queue.qsize;
  output_queue.avail->ring[index] = buf_index;
  mem_barrier; // section 3.2.1.3.1
  output_queue.avail->idx++;
  mem_barrier; // section 3.2.1.4.1

  // notify the device that there's been a change
  out_word(iobase+0x10,1);

  // must not return or the stack will get destroyed
  while (1);
  crash(0x43434343);
}

// not actually functions!
extern void heap_size();
extern void heap_start();
extern void heap_end();

uint32_t given_pages = 0;
uint8_t* gimme_memory(uint32_t pages) {
  if ((uint32_t)heap_start & 0x00000FFF) { // this check isn't necessary, it's here for my own sanity
    crash(0xBAADF00D);
  }
  if (pages + given_pages > (uint32_t)heap_size) {
    crash(0x8BADF00D);
  }
  uint8_t* res = (uint8_t*)((uint32_t)heap_start + (given_pages<<12));
  given_pages += pages;
  return res;
}
