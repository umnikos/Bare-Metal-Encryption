#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void mov_to_eax(uint32_t val);

uint32_t kernel_main() {
  mov_to_eax(0xCAFEBABE);
}
