/*
 * linker.ld
 *
 * Produces a flat binary loaded at physical address 0x10000.
 * entry.o must come first so _start lands exactly at 0x10000.
 */

OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    . = 0x10000;

    .text : {
        entry.o (.text)     /* _start must be the very first byte */
        *(.text)
    }

    .rodata : { *(.rodata) }

    .data   : { *(.data)   }

    .bss    : {
        *(.bss)
        *(COMMON)
    }
}
