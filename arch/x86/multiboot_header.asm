section .multiboot
align 4
    dd 0x1BADB002                          ; Magic number
    dd 0x00000000                          ; Flags (none)
    dd -(0x1BADB002)                       ; Checksum