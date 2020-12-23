section .text
global mov_to_eax
mov_to_eax:     mov eax, dword [esp+4]
                ret
