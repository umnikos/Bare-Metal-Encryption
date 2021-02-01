#include "prelude.h"

// not actually functions!
extern void heap_size();
extern void heap_start();
extern void heap_end();

u8* gimme_memory(u32 pages) {
  static u32 given_pages = 0;
  if ((u32)heap_start & 0x00000FFF) { // this check isn't necessary, it's here for my own sanity
    crash(0xBAADF00D);
  }
  if (pages + given_pages > (u32)heap_size) {
    crash(0x8BADF00D);
  }
  u8* res = (u8*)((u32)heap_start + (given_pages<<12));
  given_pages += pages;
  return res;
}
