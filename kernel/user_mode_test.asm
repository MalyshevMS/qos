[bits 32]
global user_mode_test

hello:
    mov eax, 1
    mov ebx, text
    int 0x80 ; Print "Hello, World!"
    ret

    

user_mode_test:
    call hello

    mov eax, 3
    int 0x80 ; Exit
    jmp $

text:
    db "Hello, World!", 10, 0