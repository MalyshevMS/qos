[bits 32]
global user_mode_test

user_mode_test:
    mov eax, 2
    mov ebx, text
    int 0x80
    jmp $

text:
    db "Hello, World!", 0