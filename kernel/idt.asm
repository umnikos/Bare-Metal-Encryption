section .data
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
                pushad
                mov al, 0x20
                out 0x20, al
                popad
                iret
idt_handler2:
                pushad
                mov al, 0x20
                out 0xA0, al
                out 0x20, al
                popad
                iret

extern fill_idt
global init_idt
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
