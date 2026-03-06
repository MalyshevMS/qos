[bits 16]
[org 0x7c00]

start:
    ; --- Загрузка ядра ---
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000      ; ES:BX = 0x10000
    
    mov ah, 0x02        ; Чтение секторов
    mov al, 20          ; Читаем 20 секторов (до 10KB)
    mov ch, 0           
    mov cl, 2           ; Сектор 2
    mov dh, 0           
    mov dl, 0x80        
    int 0x13
    
    ; --- Включение Protected Mode ---
    cli
    lgdt [gdt_ptr]
    
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp 0x08:kernel32   ; Дальний прыжок в 32-битный код

[bits 32]
kernel32:
    ; Настройка сегментов
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000    ; Стек
    
    call 0x10000    ; Вызов C функции
    
hang_loop:
    hlt
    jmp hang_loop

; --- GDT (Глобальная таблица дескрипторов) ---
; Вынесена в конец, чтобы не мешать расчетам кода
gdt_start:
    dq 0                        ; Дескриптор 0 (пустой)
    dq 0x00CF9A000000FFFF       ; Дескриптор 1 (Код)
    dq 0x00CF92000000FFFF       ; Дескриптор 2 (Данные)
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1  ; Размер
    dd gdt_start                ; Адрес

; --- Заполнение до 512 байт и подпись ---
times 510-($-$$) db 0
dw 0xAA55