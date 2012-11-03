    @ This patches the built-in DSO Quad bios to call AlterBIOS
    @ for filesystem routines.

    .thumb
    .syntax unified
    
    .ifndef ALTERBIOS_BASE
    .equ ALTERBIOS_BASE,0x08044000
    .endif

    .text

    @ __GetDev_SN
    .org 0x023C
    b.w ALTERBIOS_BASE + 1 + 5 * 4 
