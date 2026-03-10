[bits 16]
[org 0x7c00]

start:
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000
    
    mov ah, 0x02
    mov al, 20
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    int 0x13
    
    cli
    lgdt [gdt_ptr]
    
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp 0x08:kernel32

[bits 32]
kernel32:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    
    jmp 0x10000 ; Real kernel address

gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 508-($-$$) db 0
dw 0x7141
dw 0xAA55