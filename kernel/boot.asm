; reference: https://wiki.osdev.org/Bare_Bones_with_NASM

MBALIGN         equ  1 << 0                     ; align loaded modules on page boundaries
MEMINFO         equ  1 << 1                     ; provide memory map
FLAGS           equ  MBALIGN | MEMINFO          ; this is the Multiboot 'flag' field
MAGIC_NUMBER    equ 0x1BADB002                  ; define the magic number constant
CHECKSUM        equ -(MAGIC_NUMBER + FLAGS)     ; calculate the checksum
                                                ; (magic number + checksum + flags should equal 0)

; --- STACK ---

section .bss
align 16
stack_bottom:   resb 16384
stack_top:

; --- BOOT ---

section .multiboot
align 4
                dd MAGIC_NUMBER                 ; write the magic number to the machine code,
                dd FLAGS                        ; the flags,
                dd CHECKSUM                     ; and the checksum

section .text

global _start
extern disable_interrupts
extern enable_interrupts
extern kernel_main
_start:
                call disable_interrupts         ; disable interrupts while we're setting up stuff

                mov esp, stack_top              ; setup the stack (needed for C)

extern init_gdt
                call init_gdt

extern init_idt
                call init_idt

                call enable_interrupts          ; enable interrupts again
                call kernel_main
global halt
halt:           call disable_interrupts
.loop:          hlt
                jmp .loop

