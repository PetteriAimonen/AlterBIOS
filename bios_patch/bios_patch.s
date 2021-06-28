    @ This patches the built-in DSO Quad bios to call AlterBIOS
    @ for filesystem routines.

    .thumb
    .syntax unified
    
    .ifndef ALTERBIOS_BASE
    .equ ALTERBIOS_BASE,0x08009400
    .endif

    .text

    @ __GetDev_SN
    .org 0x023C
    b.w ALTERBIOS_BASE + 1 + 4 * 5

    @ __OpenFileWr
    .org 0x0288
    b.w ALTERBIOS_BASE + 1 + 4 * 6

    @ __OpenFileRd
    .org 0x028C
    b.w ALTERBIOS_BASE + 1 + 4 * 7 

    @ __ReadFileSec
    .org 0x0290
    b.w ALTERBIOS_BASE + 1 + 4 * 8 

    @ __ProgFileSec
    .org 0x0294
    b.w ALTERBIOS_BASE + 1 + 4 * 9 
 
    @ __CloseFile
    .org 0x0298
    b.w ALTERBIOS_BASE + 1 + 4 * 10 
