; reference: https://wiki.osdev.org/Bare_Bones_with_NASM

MBALIGN         equ  1 << 0                     ; align loaded modules on page boundaries
MEMINFO         equ  1 << 1                     ; provide memory map
FLAGS           equ  MBALIGN | MEMINFO          ; this is the Multiboot 'flag' field
MAGIC_NUMBER    equ 0x1BADB002                  ; define the magic number constant
CHECKSUM        equ -(MAGIC_NUMBER + FLAGS)     ; calculate the checksum
                                                ; (magic number + checksum + flags should equal 0)

section .bss
align 16
stack_bottom:   resb 16384
stack_top:

section .multiboot
align 4
                dd MAGIC_NUMBER                 ; write the magic number to the machine code,
                dd FLAGS                        ; the flags,
                dd CHECKSUM                     ; and the checksum

section .text
global _start:function (start_end - _start)
global loop:
_start:
                mov eax, 0xCAFEBABE
loop:           nop
                jmp loop                        ; loop forever
start_end:
