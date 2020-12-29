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

; --- GDT ---

section .data
; references: https://wiki.osdev.org/GDT_Tutorial and https://wiki.osdev.org/Global_descriptor_table
gdtr:           dw gdt_end-gdt-1
                dd gdt
gdt:
                ; null segment (0x00) (has to be empty)
                dw 0                            ; limit 0-15
                dw 0                            ; base 0-15
                db 0                            ; base 16-23
                db 0                            ; type
                db 0                            ; limit 16-19 | flags
                db 0                            ; base 24-31

                ; code segment (0x08)
                dw 0xFFFF
                dw 0
                db 0
                db 0x9A
                db 0xCF
                db 0

                ; data segment (0x10)
                dw 0xFFFF
                dw 0
                db 0
                db 0x92
                db 0xCF
                db 0
gdt_end:

; --- IDT ---

idtr:           dw idt_end-idt-1
                dd idt
section .bss
global idt
idt:
                resb 8*256
idt_end:
section .text
global idt_handler1
global idt_handler2
idt_handler1:
                pusha
                mov al, 0x20
                out 0x20, al
                popa
                iret
idt_handler2:
                pusha
                mov al, 0x20
                out 0xA0, al
                out 0x20, al
                popa
                iret

extern fill_idt
init_idt:
                call fill_idt                   ; too complicated to do in assembly
                lidt [idtr]

global disable_interrupts
disable_interrupts:
                cli
                ret
global enable_interrupts
enable_interrupts:
                sti
                ret

extern virtio_handler
global virtio_handler_prelude
virtio_handler_prelude:
                pusha
                call virtio_handler
                mov al, 0x20
                out 0xA0, al
                out 0x20, al
                popa
                iret

; --- BOOT ---

section .multiboot
align 4
                dd MAGIC_NUMBER                 ; write the magic number to the machine code,
                dd FLAGS                        ; the flags,
                dd CHECKSUM                     ; and the checksum

section .text
init_gdt:
                lgdt [gdtr]                     ; load global descriptor table
                jmp 0x08:.reload_cs             ; a far-jump to change cs segment register
.reload_cs:
                mov ax, 0x10                    ; change every other segment register
                mov ds, ax
                mov es, ax
                mov fs, ax
                mov gs, ax
                mov ss, ax
                ret

global _start
extern kernel_main
_start:
                call disable_interrupts         ; disable interrupts while we're setting up stuff

                mov esp, stack_top              ; setup the stack (needed for C)

                call init_gdt

                call init_idt

                call enable_interrupts          ; enable interrupts again
                call kernel_main
global halt
halt:           call disable_interrupts
.loop:          hlt
                jmp .loop

; --- HEAP ---

; there are cleaner solutions than this, but eh.
section .heap
align 4096
global heap_start
global heap_end
global heap_size
heap_size:      equ 32
heap_start:
                resb 4096*heap_size
heap_end:

global waiting
waiting:
                jmp $
                ret
