#include "prelude.h"

#define heap_size 32
static u8 heap_start[heap_size*4096] __attribute__((aligned(4096)));

u8* gimme_memory(u32 pages) {
  static u32 given_pages = 0;
  if ((u32)heap_size != 32) {
    crash("WRONG HEAP SIZE!\n");
  }
  if ((u32)heap_start & 0x00000FFF) { // this check isn't necessary, it's here for my own sanity
    crash("MISALIGNED HEAP\n");
  }
  if (pages + given_pages > (u32)heap_size) {
    crash("HEAP OVERFLOW\n");
  }
  u8* res = heap_start + (given_pages<<12);
  given_pages += pages;
  return res;
}
