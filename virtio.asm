section .text
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

