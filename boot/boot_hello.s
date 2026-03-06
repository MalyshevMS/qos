[bits 16]
[org 0x7c00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ax, 0xB800
    mov es, ax

    mov si, msg
    mov di, 0

print_loop:
    lodsb
    cmp al, 0
    je done

    stosb
    mov byte [es:di], 0x01
    inc di

    jmp print_loop

done:
    jmp $

msg db 'Hello World!', 0

times 510-($-$$) db 0
dw 0xAA55