#include "prelude.h"

#define heap_size 32
static u8 heap_start[heap_size*4096] __attribute__((aligned(4096))) = {0};

u8* gimme_memory(u32 pages) {
  if (sizeof(u8*) != 4) {
    crash("POINTER SIZE IS NOT 4??");
  }
  static u32 given_pages = 0;
  if ((uptr)heap_size != 32) {
    crash("WRONG HEAP SIZE!\n");
  }
  if ((uptr)heap_start & 0x00000FFF) { // this check isn't necessary, it's here for my own sanity
    crash("MISALIGNED HEAP\n");
  }
  if (pages + given_pages > (uptr)heap_size) {
    crash("HEAP OVERFLOW\n");
  }
  u8* res = heap_start + (given_pages<<12);
  given_pages += pages;
  return res;
}
