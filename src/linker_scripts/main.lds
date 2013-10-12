MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    
    /* We use the top 5 KB of the extra RAM */
    ram (rwx) : ORIGIN = 0x2000EC00, LENGTH = 5K
}

SECTIONS
{
    .bios 0x08004200 (NOLOAD) : {
      build/BIOS.o
    }
    
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
