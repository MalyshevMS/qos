[bits 32]
global user_mode_test

hello:
    mov eax, 1
    mov ebx, text
    int 0x80 ; Print "Hello, World!"
    ret

exit:
    mov eax, 3
    int 0x80 ; Exit

sleep:
    mov ebx, eax
    mov eax, 2
    int 0x80
    ret

user_mode_test:
    mov eax, 10
    call sleep

    call hello

    mov eax, 1000
    call sleep

    call hello
    
    call exit
    jmp $ ; Not necessary, but recomended

text:
    db "Hello, World!", 10, 0