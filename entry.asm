; entry.asm – 32-bit entry point, linked first so it lands at 0x10000
[BITS 32]
[GLOBAL _start]
[EXTERN kernel_main]

_start:
    call kernel_main
    ; If kernel_main ever returns, just halt
.hang:
    cli
    hlt
    jmp .hang