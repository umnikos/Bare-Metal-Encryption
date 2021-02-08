section .text

extern fibonacci
fibonacci_delay equ 10
; TODO - add fibonacci delay to every out function

global nothing
nothing:        ret                             ; for debugging purposes

global out_byte                                 ; out_byte(uint16_t port, uint8_t data)
out_byte:
                mov dx, word [esp+4]
                mov al, byte [esp+8]
                out dx, al
                ret

global out_word                                 ; out_byte(uint16_t port, uint16_t data)
out_word:
                mov dx, word [esp+4]
                mov ax, word [esp+8]
                out dx, ax
                ret

global out_dword                                ; out_byte(uint16_t port, uint32_t data)
out_dword:
                mov dx, word [esp+4]
                mov eax, dword [esp+8]
                out dx, eax
                ret

global in_byte                                  ; uint8_t in_byte(uint16_t port)
in_byte:
                mov dx, [esp+4]
                xor eax, eax
                in al, dx
                ret

global in_word                                  ; uint16_t in_byte(uint16_t port)
in_word:
                mov dx, [esp+4]
                xor eax, eax
                in ax, dx
                ret

global in_dword                                 ; uint32_t in_byte(uint16_t port)
in_dword:
                mov dx, [esp+4]
                in eax, dx
                ret

