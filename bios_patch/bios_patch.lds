MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 512K
}

SECTIONS
{
    .text 0x08004000 : {
        build/bios_patch.o(.text)
    }
}
