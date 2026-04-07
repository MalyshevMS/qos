[bits 32]
global irq1_handler
extern keyboard_interrupt_handler
extern pic_send_eoi

irq1_handler:
    pusha
    call keyboard_interrupt_handler
    
    ; Send EOI to PIC
    mov eax, 1
    call pic_send_eoi
    
    popa
    iretd