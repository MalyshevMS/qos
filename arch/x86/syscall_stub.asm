[bits 32]
global syscall_stub
extern syscall_handler

syscall_stub:
    push 0
    push 0x80
    pusha ; Save EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    
    mov ax, ds ; Save DS
    push eax

    mov ax, 0x10 ; Load Kernel Data Segment
    mov ds, ax
    mov es, ax

    push esp
    call syscall_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax

    popa ; Restore regiters
    add esp, 8
    iretd ; Return to User Mode