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

section .text

global init_gdt
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
