[bits 32]

; Экспортируем таблицу указателей для C++
global irq_stub_table
extern irq_common_handler

%macro IRQ_STUB 1
global irq_stub_%1
irq_stub_%1:
    push 0          ; Фиктивный код ошибки
    push %1 + 32    ; Номер прерывания (32, 33...)
    jmp irq_common_stub
%endmacro

; Генерируем 16 заглушек для IRQ0-IRQ15
%assign i 0
%rep 16
    IRQ_STUB i
%assign i i+1
%endrep

irq_common_stub:
    pusha           ; Сохраняем EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    
    mov ax, ds      ; Сохраняем текущий сегмент данных
    push eax

    mov ax, 0x10    ; Загружаем сегмент данных ядра (обычно 0x10 в GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Передаем указатель на структуру Registers в C++
    call irq_common_handler
    add esp, 4      ; Очищаем стек от указателя

    pop eax         ; Восстанавливаем сегменты
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Восстанавливаем регистры общего назначения
    add esp, 8      ; Очищаем стек от номера прерывания и кода ошибки
    iretd           ; Возврат из прерывания

; Таблица адресов всех заглушек
section .data
irq_stub_table:
%assign i 0
%rep 16
    dd irq_stub_%[i]
%assign i i+1
%endrep