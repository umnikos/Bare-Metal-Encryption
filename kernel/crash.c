#include "prelude.h"

extern void halt();

void crash(const char* msg) {
  debug("CRASH: ");
  debug(msg);
  halt();
}

