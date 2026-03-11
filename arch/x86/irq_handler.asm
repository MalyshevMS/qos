[bits 32]
global irq1_handler
extern keyboard_interrupt_handler

irq1_handler:
    pusha
    call keyboard_interrupt_handler
    popa
    iretd