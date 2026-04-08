%assign KERNEL_CODE_SEG 0x08
%assign KERNEL_DATA_SEG 0x10

extern gdt_init

section .text
global gdt_load

gdt_load:
    mov eax, [esp + 4]
    lgdt [eax]
    
    ; Far jump to reload CS
    jmp KERNEL_CODE_SEG:.reload_cs

.reload_cs:
    ; Reload data segment registers
    mov eax, KERNEL_DATA_SEG
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    ret
