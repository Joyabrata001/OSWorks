bits 16     ;   for backwards compatibility
org 0x7c00  ;   BIOS loads the boot sector to the address 0x7c00


times 510-($-$$) db 0
dw 0xAA55

; Compiled using:
; nasm mbr.asm -f bin -o mbr.bin
; Converted to .img file