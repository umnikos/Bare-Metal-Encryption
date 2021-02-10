#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"

void terminal_initialize();

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");
  rsa_demo();
}

#include "io-vga.c"
#include "misc.c"
