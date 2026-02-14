/* ba.ld - Minimal ELF header */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

PHDRS
{
    text PT_LOAD FILEHDR PHDRS;
}

SECTIONS
{
    . = 0x08048000 + SIZEOF_HEADERS;
    
    .text : {
        *(.text*)
        *(.rodata*)
        *(.data*)
        . = ALIGN(16);
    } :text
    
    /DISCARD/ : {
        *(*)
    }
}