[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x1000

; =============================================
; Stage 1: Real Mode — Load kernel from disk
; =============================================
start:
    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00          ; Stack grows down from bootloader

    ; Save boot drive number (BIOS passes it in dl)
    mov [BOOT_DRIVE], dl

    ; Print boot message
    mov si, MSG_BOOT
    call print_string_16

    ; Load kernel from disk
    call load_kernel

    ; Switch to protected mode
    call switch_to_pm

    jmp $                    ; Should never reach here

; =============================================
; Print a null-terminated string (16-bit mode)
; =============================================
print_string_16:
    pusha
.loop:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0e
    int 0x10
    jmp .loop
.done:
    popa
    ret

; =============================================
; Load kernel sectors from disk into memory
; =============================================
load_kernel:
    mov si, MSG_LOAD
    call print_string_16

    mov bx, KERNEL_OFFSET   ; Load kernel to 0x1000
    mov dh, 15              ; Read 15 sectors (plenty of room)
    mov dl, [BOOT_DRIVE]

    mov ah, 0x02             ; BIOS read sectors function
    mov al, dh               ; Number of sectors
    mov ch, 0                ; Cylinder 0
    mov cl, 2                ; Start from sector 2 (sector 1 is bootloader)
    mov dh, 0                ; Head 0

    int 0x13                 ; Call BIOS
    jc disk_error            ; Jump if carry flag set (error)

    ret

disk_error:
    mov si, MSG_DISK_ERR
    call print_string_16
    jmp $

; =============================================
; GDT — Global Descriptor Table
; =============================================
gdt_start:
gdt_null:                    ; Mandatory null descriptor
    dd 0x0
    dd 0x0

gdt_code:                   ; Code segment descriptor
    dw 0xffff                ; Limit (bits 0-15)
    dw 0x0                   ; Base (bits 0-15)
    db 0x0                   ; Base (bits 16-23)
    db 10011010b             ; Access byte: present, ring 0, code, readable
    db 11001111b             ; Flags: 4KB granularity, 32-bit + limit (bits 16-19)
    db 0x0                   ; Base (bits 24-31)

gdt_data:                   ; Data segment descriptor
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b             ; Access byte: present, ring 0, data, writable
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                 ; GDT address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; =============================================
; Switch to 32-bit Protected Mode
; =============================================
switch_to_pm:
    cli                      ; Disable interrupts
    lgdt [gdt_descriptor]    ; Load GDT

    mov eax, cr0
    or eax, 0x1              ; Set protected mode bit
    mov cr0, eax

    jmp CODE_SEG:init_pm     ; Far jump to flush pipeline

[bits 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000         ; Set up stack
    mov esp, ebp

    call KERNEL_OFFSET       ; Jump to kernel!

    jmp $                    ; Hang if kernel returns

; =============================================
; Data
; =============================================
BOOT_DRIVE db 0
MSG_BOOT   db "Booting Minimus...", 13, 10, 0
MSG_LOAD   db "Loading kernel...", 13, 10, 0
MSG_DISK_ERR db "Disk read error!", 0

; =============================================
; Boot sector padding
; =============================================
times 510-($-$$) db 0
dw 0xaa55