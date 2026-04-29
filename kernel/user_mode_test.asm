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

user_mode_test:
    ; mov eax, 2
    ; mov ebx, 1000
    ; int 0x80 ; sleep

    call hello
    
    call exit
    jmp $ ; Not necessary, but recomended

text:
    db "Hello, World!", 10, 0