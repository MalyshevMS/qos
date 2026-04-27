[bits 32]
section .text
global tss_load

tss_load:
    mov ax, 0x2B
    ltr ax
    ret