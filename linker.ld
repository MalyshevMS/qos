ENTRY(kernel_main)

PHDRS
{
    text PT_LOAD FLAGS(5);   /* R + X */
    rodata PT_LOAD FLAGS(4); /* R     */
    data PT_LOAD FLAGS(6);   /* R + W */
}

SECTIONS
{
    . = 0x10000;

    /* 
        Hardcode kernel to be exactly at 1M 
        (you should use "__attribute__((section(".kernel_main")))" before your entry)
    */
    .kernel_main : {
        *(.kernel_main)
    } :text

    .text : {
        *(.text)
        *(.text.*)
    } :text

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } :rodata

    .data : {
        *(.data)
        *(.data.*)
    } :data

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } :data
}