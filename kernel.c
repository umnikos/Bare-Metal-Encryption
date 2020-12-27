#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void mov_to_eax(uint32_t val);
extern void out_byte(uint16_t port, uint8_t data);
extern void out_dword(uint16_t port, uint32_t data);
extern uint8_t in_byte(uint16_t port);
extern uint32_t in_dword(uint16_t port);

struct search_result {
  uint32_t bus;
  uint32_t device;
};
void halt();
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
void virtio_queues();
uint8_t* gimme_memory(uint32_t pages);

void kernel_main() {
  //mov_to_eax(0xCAFEBABE);

  //serial_init();
  //serial_print("Hello, World!\n");

  struct search_result virtio = pci_find_virtio();
  virtio_init(virtio);
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

  uint32_t bus = res.bus;
  uint32_t device = res.device;
  //uint16_t irq = pci_read_irq(bus, device);
  //setup_irq(irq); // I'm pretty sure this is only necessary for input

  /* from testing:
   * bar0 = c041
   * 1100 0000 0100 0001
   *
   * bar1 = 1000
   * 0001 0000 0000 0000
   */
  uint16_t iobase = pci_read_bar(bus, device, 0) & 0xFFFC; // we only take the bottom word for some reason
  uint8_t status = VIRTIO_ACKNOWLEDGE;
  out_byte(iobase+0x12, status);
  status |= VIRTIO_DRIVER;
  out_byte(iobase+0x12, status);
  uint32_t supported_features = in_dword(iobase+0x00);
  virtio_negotiate(&supported_features);
  out_dword(iobase+0x04, supported_features);
  status |= VIRTIO_FEATURES_OK;
  out_byte(iobase+0x12, status);
  if ((in_byte(iobase+0x12) & VIRTIO_FEATURES_OK) == 0) {
    // the negotiations have failed!
    status |= VIRTIO_FAILED;
    out_byte(iobase+0x12, status);
    mov_to_eax(0xBABADEAD);
    halt();
  }
  virtio_queues();
  status |= VIRTIO_DRIVER_OK;
  out_byte(iobase+0x12, status);
}

void virtio_negotiate(uint32_t* supported_features) {
  /* from testing:
   * supported_features = 79000006
   * 0111 1001 0000 0000 0000 0000 0000 0110
   */
  *supported_features &= 0xFF000000;
}

// section 4.1.5.1.3.1 tells us these need to be at a 4K boundary (X<<12)
uint8_t* input_queue;
uint8_t* output_queue;

void virtio_queues() {
  // section 5.3.2 tells us what the two queues are
  // only queue #1 is needed for output
  uint32_t q_addr = 1;

  // grab queue size

  uint8_t* test = gimme_memory(15);
  test = gimme_memory(8);
  mov_to_eax(test);

  halt();
}

// not actually functions!
extern void heap_size();
extern void heap_start();
extern void heap_end();

uint32_t given_pages = 0;
uint8_t* gimme_memory(uint32_t pages) {
  if ((uint32_t)heap_start & 0x00000FFF) { // this check isn't necessary, it's here for my own sanity
    mov_to_eax(0xBAADF00D);
    halt();
  }
  if (pages + given_pages > (uint32_t)heap_size) {
    mov_to_eax(0x8BADF00D);
    halt();
  }
  uint8_t* res = (uint8_t*)((uint32_t)heap_start + (given_pages<<12));
  given_pages += pages;
  return res;
}
