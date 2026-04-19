[bits 32]
extern exception_common_handler

%macro ISR_NOERRCODE 1
global isr_stub_%1
isr_stub_%1:
    push 0
    push %1
    jmp exception_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr_stub_%1
isr_stub_%1:
    push %1
    jmp exception_common_stub
%endmacro

ISR_NOERRCODE 0  ; Divide by Zero
ISR_NOERRCODE 1  ; Debug
ISR_NOERRCODE 2  ; Non Maskable Interrupt
ISR_NOERRCODE 3  ; Breakpoint
ISR_NOERRCODE 4  ; Into Detected Overflow
ISR_NOERRCODE 5  ; Out of Bounds
ISR_NOERRCODE 6  ; Invalid Opcode
ISR_NOERRCODE 7  ; No Coprocessor
ISR_ERRCODE   8  ; Double Fault
ISR_NOERRCODE 9  ; Coprocessor Segment Overrun
ISR_ERRCODE   10 ; Bad TSS
ISR_ERRCODE   11 ; Segment Not Present
ISR_ERRCODE   12 ; Stack Fault
ISR_ERRCODE   13 ; General Protection Fault
ISR_ERRCODE   14 ; Page Fault
ISR_NOERRCODE 15 ; Unknown Interrupt
ISR_NOERRCODE 16 ; Coprocessor Fault
ISR_ERRCODE   17 ; Alignment Check
ISR_NOERRCODE 18 ; Machine Check
ISR_NOERRCODE 19 ; Reserved
ISR_NOERRCODE 20 ; Reserved
ISR_NOERRCODE 21 ; Reserved
ISR_NOERRCODE 22 ; Reserved
ISR_NOERRCODE 23 ; Reserved
ISR_NOERRCODE 24 ; Reserved
ISR_NOERRCODE 25 ; Reserved
ISR_NOERRCODE 26 ; Reserved
ISR_NOERRCODE 27 ; Reserved
ISR_NOERRCODE 28 ; Reserved
ISR_NOERRCODE 29 ; Reserved
ISR_NOERRCODE 30 ; Reserved
ISR_NOERRCODE 31 ; Reserved

exception_common_stub:
    pusha ; save registers
    mov ax, ds
    push eax ; save data segment

    mov ax, 0x10 ; load kernel segment
    mov ds, ax
    mov es, ax

    push esp ; pointer to Arch::x86::Registers
    call exception_common_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    popa
    add esp, 8
    iretd

section .data
global exception_stub_table
exception_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%[i]
%assign i i+1
%endrep