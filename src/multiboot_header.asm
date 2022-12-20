section .multiboot_header
header_start:
    dd 0xe85250d6                ; magic number
    dd 0                         ; protected mode code
    dd header_end - header_start ; header length

    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
    dw 0    ; type
    ; 0x1 for page align, 0x2 for memory info, 0x4 for video mode
    dw (0x1 | 0x2 | 0x4)  ; flags
    dd 8    ; size
header_end:
