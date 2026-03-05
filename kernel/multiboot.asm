section .multiboot
align 8

MB2_MAGIC        equ 0xE85250D6
MB2_ARCH         equ 0
MB2_HEADER_LEN   equ header_end - header_start
MB2_CHECKSUM     equ -(MB2_MAGIC + MB2_ARCH + MB2_HEADER_LEN)

header_start:

dd MB2_MAGIC
dd MB2_ARCH
dd MB2_HEADER_LEN
dd MB2_CHECKSUM

dw 0
dw 0
dd 8

header_end: