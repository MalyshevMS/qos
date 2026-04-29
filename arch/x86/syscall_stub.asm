[bits 32]
global syscall_stub
extern syscall_handler

syscall_stub:
    push 0
    push 0x80
    pusha ; Save EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    
    mov ax, ds
    push eax

    mov ax, 0x10 ; Load Kernel Data Segment
    mov ds, ax
    mov es, ax

    push esp ; Pointer to struct Rgisters
    call syscall_handler
    mov esp, eax ; Reload stack (yet changes only on syscall 3)

    pop eax
    mov ds, ax
    mov es, ax

    popa ; Restore regiters
    add esp, 8
    iretd ; Return to User Mode