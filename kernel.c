#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void mov_to_eax(uint32_t val);
void out_byte(uint16_t port, uint8_t data);
void out_dword(uint16_t port, uint32_t data);
uint8_t in_byte(uint16_t port);
uint32_t in_dword(uint16_t port);

void serial_init(); // TODO
void serial_print(char* str);
void serial_send(char c); // TODO
uint16_t pci_read_config(uint32_t bus, uint32_t device, uint32_t func, uint32_t offset);
uint16_t pci_read_vendor(uint32_t bus, uint32_t device);
uint16_t pci_read_deviceid(uint32_t bus, uint32_t device);
struct search_result {
  uint32_t bus;
  uint32_t device;
  uint16_t subsystem;
};
struct search_result pci_find_virtio();

void kernel_main() {
  mov_to_eax(0xCAFEBABE);

  //serial_init();
  //serial_print("Hello, World!\n");

  struct search_result virtio = pci_find_virtio();
  mov_to_eax(virtio.subsystem);
}

/*
void serial_print(char* str) {
  while (*str != '\0') {
    serial_send(*str);
    str++;
  }
}
*/

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

uint16_t pci_read_vendor(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0);
}

uint16_t pci_read_deviceid(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,2);
}

uint16_t pci_read_subsystem(uint32_t bus, uint32_t device) {
  return pci_read_config(bus,device,0,0x2E);
}

struct search_result pci_find_virtio() {
  struct search_result result;
  for(uint32_t bus=0; bus<256; bus++) {
    for(uint32_t device=0; device<32; device++) {
      uint16_t deviceid = pci_read_deviceid(bus, device);
      // there should be a subsystem check but I don't know what the subsystem number of virtio-serial is!
      if (deviceid >= 0x1000 &&
          deviceid <= 0x103F &&
          pci_read_vendor(bus,device) == 0x1AF4) {
        result.subsystem = pci_read_subsystem(bus,device); // so I am going to log it in the output
        result.bus = bus;
        result.device = device;
        return result;
      }
    }
  }
  // if nothing has been found then there's nothing to do but hang
  while (1);
}
