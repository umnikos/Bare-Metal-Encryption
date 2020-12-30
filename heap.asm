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
