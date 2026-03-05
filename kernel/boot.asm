global _start
extern kernel_main

section .text
bits 64

_start:
    ; Test if the grub is loading (should be X in the top left corner of VGA)
    mov rax, 0xB8000
    mov byte [rax], 'X'
    mov byte [rax+1], 0x0F

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang