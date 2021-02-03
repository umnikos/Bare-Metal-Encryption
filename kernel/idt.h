#pragma once
#include "prelude.h"

// https://wiki.osdev.org/IDT
struct idt_entry {
  u16 offset_1; // offset bits 0..15
  u16 selector; // code segment selector in GDT
  u8 zero; // full of 0s for legacy reasons
  u8 type_attr; // settings
  u16 offset_2; // offset bits 0..16
} __attribute__((packed));
