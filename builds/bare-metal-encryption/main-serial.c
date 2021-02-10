#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../serial/serial.h"

void terminal_initialize();

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  serial_initialize();

  rsa_main();
}

#include "io-serial.c"
#include "misc.c"
