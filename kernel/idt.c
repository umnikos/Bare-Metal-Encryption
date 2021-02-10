#include "prelude.h"
#include "idt.h"

extern struct idt_entry idt[];
extern void idt_handler0();
extern void idt_handler1();
extern void idt_handler2();
extern void enable_interrupts();
extern void disable_interrupts();

void fill_idt() {
  // https://wiki.osdev.org/8259_PIC
  //
  // 0x20 = master command
  // 0x21 = master data
  // 0xA0 = slave command
  // 0xA1 = slave data
  out_byte(0x20, 0x11); // start initialization sequence
  out_byte(0xA0, 0x11);

  out_byte(0x21, 0x20); // master PIC offset 0x20
  out_byte(0xA1, 0x28); // slave PIC offset 0x28 (0x20+8)
  out_byte(0x21, 0x04); // tell master to cascane to slave through IRQ2
  out_byte(0xA1, 0x02); // tell slave its cascade identity
  out_byte(0x21, 0x01); // 8086 mode
  out_byte(0xA1, 0x01);
  out_byte(0x21, 0); // set masks to 0 (all interrupts enabled)
  out_byte(0xA1, 0);

  for (u32 i=0; i<256; i++) {
    struct idt_entry ih;
    u32 handler;
    switch (i) {
      case 0x20: // PIT
      case 0x21: // keyboard
        handler=(uptr)idt_handler1;
        break;
      default:
        handler=(uptr)idt_handler0;
        break;
    }
    ih.offset_1 = (u16)handler;
    ih.offset_2 = (u16)(handler >> 16);

    ih.zero = 0;
    ih.selector = 0x08;
    ih.type_attr = 0x8E;
    idt[i] = ih;
  }
}
