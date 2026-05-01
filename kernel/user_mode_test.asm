[bits 32]
global user_mode_test

user_mode_test:
    mov eax, 2
    mov ebx, 5000
    int 0x80 ; Sleep

    mov eax, 3
    mov ebx, text
    int 0x80 ; Print

    mov eax, 2
    mov ebx, 500
    int 0x80 ; Sleep

    mov eax, 1
    mov ebx, 1234
    int 0x80 ; Exit
    jmp $ ; Not necessary, but recomended

text:
    db "Hello, World!", 10, 0