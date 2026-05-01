[bits 32]
global user_mode_test

user_mode_test:
    mov eax, 1
    mov ebx, 1234
    int 0x80
    jmp $ ; Not necessary, but recomended

text:
    db "Hello, World!", 10, 0