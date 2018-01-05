ENTRY (_loader)

SECTIONS
{
    . = 0x00100000;

    .text :
    {
        *(.text)
    }

    .rodata ALIGN (0x200) :
    {
        *(.rodata)
    }

    .data ALIGN (0x200) :
    {
        *(.data)
    }

    .bss :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }
    __end = .;
}
