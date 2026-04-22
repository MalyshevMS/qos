[bits 32]

global irq_stub_table
extern irq_common_handler

%macro IRQ_STUB 1
global irq_stub_%1
irq_stub_%1:
    push 0
    push %1 + 32
    jmp irq_common_stub
%endmacro

%assign i 0
%rep 16
    IRQ_STUB i
%assign i i+1
%endrep

irq_common_stub:
    pusha ; Save registers
    
    mov ax, ds ; Save Data Segment
    push eax

    mov ax, 0x10 ; Load from GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_common_handler
    mov esp, eax

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa ; Restore registers
    add esp, 8 ; Clear stack and return
    iretd

section .data
irq_stub_table:
%assign i 0
%rep 16
    dd irq_stub_%[i]
%assign i i+1
%endrep