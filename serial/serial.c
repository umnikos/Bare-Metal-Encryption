#include "../kernel/prelude.h"
#include "serial.h"

void i_to_str(u32 num, char* buf, u32 size);

// https://littleosbook.github.io/#the-serial-ports
// https://wiki.osdev.org/Serial_Ports#Example_Code
// https://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming

#define COM1 0x3F8
#define serial_divisor 0x0C

#define serial_data(base) (base+0)
#define serial_interrupt(base) (base+1)
#define serial_divisor_low(base) (base+0)
#define serial_divisor_high(base) (base+1)
#define serial_fifo_control(base) (base+2)
#define serial_line_control(base) (base+3)
#define serial_modem_control(base) (base+4)
#define serial_line_status(base) (base+5)

void serial_initialize() {
  out_byte(serial_interrupt(COM1), 0); // disable interrupts while configuring
  out_byte(serial_line_control(COM1), 0x80); // enable DLAB to access clock divisor
  out_byte(serial_divisor_low(COM1), serial_divisor); // set the divisor's low bits
  out_byte(serial_divisor_high(COM1), 0); // high bits not needed
  out_byte(serial_line_control(COM1),
  //          >>  data length of 8 bits
  //         >||  1 stop bit
  //      >>>|||  no parity checking
  //     >||||||  break control(?) disabled
  //    >|||||||  disable DLAB
      0b00000011);
  out_byte(serial_fifo_control(COM1),
  //           >  enable fifo
  //          >|  clear receiver fifo
  //         >||  clear transmission fifo
  //        >|||  no dma (direct memory access)
  //      > ||||  16 byte buffer instead of 64 (for 16750)
  //    >>| ||||  14 byte threshold
  //    |||>||||  (reserved)
      0b11000111);
  out_byte(serial_modem_control(COM1),
  //           >  data terminal ready
  //          >|  request to send
  //         >||  auxiliary output 1
  //        >|||  auxiliary output 2 (needed for interrupts)
  //       >||||  loopback mode (for testing)
  //      >|||||  16750 autoflow control (relatively new)
  //    >>||||||  (reserved)
      0b00001011);

  // FIXME - do a test with loopback

  out_byte(serial_interrupt(COM1),
  // enable interrupts for:
  //           >  available data
  //          >|  transmitter empty
  //         >||  break/error
  //        >|||  status change
  //    >>>>||||  (reserved)
      0b00000000);
}

void serial_send_char(char c) {
    while ((in_byte(serial_line_status(COM1)) & 0x20) == 0);
    out_byte(serial_data(COM1), (u8)c);
}

void serial_send(const char* s) {
  for (; *s != '\0'; s++) {
    if (*s == '\n') {
      serial_send_char('\r');
    }
    serial_send_char(*s);
  }
}

void serial_receive(char* s, u32 size) {
  u32 count = 0;
  while (true) {
    if (count == size) {
      serial_send("\n");
      *s = '\0';
      return;
    }
    while ((in_byte(serial_line_status(COM1)) & 0x01) == 0);
    *s = (char)in_byte(serial_data(COM1));
    if (*s == 0x1B) continue;

    char debug_buf[65] = {0};
    i_to_str((u32)*s, debug_buf, 63);
    debug(debug_buf);
    debug("\n");

    count++;
    if (*s == 0x0D) {
      serial_send("\n");
      *s = '\0';
      return;
    }
    serial_send_char(*s);
    if (*s == 0x7F) {
      serial_send("\b");
    }
    if (*s == 0x08 || *s == 0x7F) {
      serial_send(" \b");
      s--;
      s--;
      count--;
      count--;
    }
    s++;
  }
}
