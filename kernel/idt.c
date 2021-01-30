#include "prelude.h"

// https://wiki.osdev.org/IDT
struct idt_entry {
  u16 offset_1; // offset bits 0..15
  u16 selector; // code segment selector in GDT
  u8 zero; // full of 0s for legacy reasons
  u8 type_attr; // settings
  u16 offset_2; // offset bits 0..16
} __attribute__((packed));

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
    if (i >= 0x20 && i < 0x28) {
      ih.offset_1 = (u32)idt_handler1 & 0xFFFF;
      ih.offset_2 = ((u32)idt_handler1 & 0xFFFF0000) >> 16;
    } else if (i >= 0x28 && i < 0x30) {
      ih.offset_1 = (u32)idt_handler2 & 0xFFFF;
      ih.offset_2 = ((u32)idt_handler2 & 0xFFFF0000) >> 16;
    } else {
      ih.offset_1 = (u32)idt_handler0 & 0xFFFF;
      ih.offset_2 = ((u32)idt_handler0 & 0xFFFF0000) >> 16;
    }
    ih.zero = 0;
    ih.selector = 0x08;
    ih.type_attr = 0x8E;
    idt[i] = ih;
  }
}

extern void virtio_handler_prelude();

void set_irq(u8 irq) {
  disable_interrupts();
  idt[irq].offset_1 = (u32)virtio_handler_prelude & 0xFFFF;
  idt[irq].offset_2 = ((u32)virtio_handler_prelude & 0xFFFF0000) >> 16;
  enable_interrupts();
}
