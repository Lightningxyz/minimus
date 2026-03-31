[org 0x7c00]
[bits 16]

start:
    mov ah, 0x0e
    mov si, message

print:
    lodsb
    cmp al, 0
    je hang
    int 0x10
    jmp print

hang:
    jmp $

message db "Booting OS...", 0

times 510-($-$$) db 0
dw 0xaa55