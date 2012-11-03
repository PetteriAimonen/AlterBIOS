MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    
    /* The applications use  0x20003000 onwards.
     * The default bios uses 0x20000000 to 0x20000FFF
     * Therefore, we use 0x20002000 to 0x20002FFF
     */
    ram (rwx) : ORIGIN = 0x20002000, LENGTH = 4K
}

SECTIONS
{
    .bios 0x08004200 (NOLOAD) : {
      build/BIOS.o
    }
    /*
    .text.bios_patch 0x0800423C : {
        build/bios_patch.o(.text.chksys)
    }
    */
    .text alterbios_base : {
        build/jump_table.o(.text)
        
        _scode = .;
        *(.text*)        /* Program code */
        _ecode = .;
        *(.rodata*)      /* Read only data */
        _etext = .;
        _sidata = _etext;
    } >rom

    .data : {
        _sdata = . ;
        *(.data)        /* Data memory */
        _edata = .;
    } >ram AT > rom

    .bss : {
        _sbss = .;
        *(.bss)         /* Zero-filled run time allocate data memory */
        _ebss = .;
    } >ram AT > rom
    
    .dummy (NOLOAD) : {
        *(.ARM.*)
        *(.debug*)
    }
}
