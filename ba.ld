OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x08048000;
    
    .text : {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
    }
}