bits 16     ;   for backwards compatibility
org 0x7c00  ;   BIOS loads the boot sector to the address 0x7c00

    mov bx, WELCOME_MSG
    call print_string

    jmp $

print_string:
    pusha
    mov ah, 0x0E
loop:
    mov al, [bx]
    cmp al, 0
    je finished

    int 0x10
    inc bx
    jmp loop
    
finished:
    mov al, 0x0A    ;   newline
    int 0x10
    mov al, 0x0D    ;   carriage return
    int 0x10
    popa
    ret

WELCOME_MSG :
db 'Booting my OS...' ,0

times 510-($-$$) db 0
dw 0xAA55