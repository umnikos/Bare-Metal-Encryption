#include "../../kernel/prelude.h"
#include "../../encryption/prelude.h"
#include "../../serial/serial.h"

void terminal_initialize();

void kernel_main() {
  terminal_initialize();
  debug("-DEBUG-\n");

  serial_initialize();
  char buf[65] = "Hello!\n";
  serial_send(buf);

  while (true) {
    serial_receive(buf,63);
    serial_send(buf);
  }
}

#include "io-vga.c"
#include "misc.c"
