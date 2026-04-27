[bits 32]
global jump_to_user

jump_to_user:
    ; [esp + 4] - entry_point
    ; [esp + 8] - user_stack

    mov ebp, [esp + 4]
    mov ecx, [esp + 8]

    ; Faking stack for IRET:
    push 0x23
    push ecx
    push 0x202
    push 0x1B
    push ebp

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iretd