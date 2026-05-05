[BITS 16]
[ORG 0x7C00]

; ─────────────────────────────────────────────
; Entry point
; ─────────────────────────────────────────────
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl

    sti

    ; Print boot message
    mov si, msg_boot
    call print_str

    ; ─────────────────────────────────────────
    ; Load kernel from disk (BIOS INT 13h)
    ; ─────────────────────────────────────────
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02        ; read sectors
    mov al, 16          ; 16 sectors (8 KB)
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; ─────────────────────────────────────────
    ; NOW switch to VGA mode 13h (safe timing)
    ; ─────────────────────────────────────────
    mov ax, 0x0013
    int 0x10

    ; ─────────────────────────────────────────
    ; Load GDT and enter protected mode
    ; ─────────────────────────────────────────
    lgdt [gdt_desc]

    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pm_entry


; ─────────────────────────────────────────────
; Real-mode helpers
; ─────────────────────────────────────────────
print_str:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    xor bh, bh
    int 0x10
    jmp print_str
.done:
    ret

disk_error:
    mov si, msg_err
    call print_str
    hlt


; ─────────────────────────────────────────────
; Data
; ─────────────────────────────────────────────
msg_boot     db "BOOT OK", 0x0D, 0x0A, 0
msg_err      db "DISK ERROR", 0x0D, 0x0A, 0
boot_drive   db 0


; ─────────────────────────────────────────────
; GDT (flat model)
; ─────────────────────────────────────────────
align 8

gdt_start:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; ─────────────────────────────────────────────
; Protected mode entry
; ─────────────────────────────────────────────
[BITS 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x9F000

    ; Jump to kernel (loaded at 0x10000)
    jmp 0x10000


; ─────────────────────────────────────────────
; Boot sector padding + signature
; ─────────────────────────────────────────────
times 510-($-$$) db 0
dw 0xAA55