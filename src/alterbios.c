#include "fatfs/ff.h"
#include "DS203/BIOS.h"

#define VERSION "AlterBIOS 1.0"

#define RGB(r, g, b) (((r)>>3)|(((g)>>2)<<5)|(((b)>>3)<<11))

FATFS fatfs;

extern unsigned long _etext;
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;

void __Init_Data(void) {
    unsigned long *src, *dst;
    /* copy the data segment into ram */
    src = &_sidata;
    dst = &_sdata;
    if (src != dst)
        while(dst < &_edata)
            *(dst++) = *(src++);

    /* zero the bss segment */
    dst = &_sbss;
    while(dst < &_ebss)
        *(dst++) = 0;
}

#define STATUS(x) __Display_Str(88, 110, RGB(0,255,0), 0, (u8*)(VERSION ": " x "                 "))

void alterbios_init()
{
    STATUS("Initializing");
 
    __Init_Data();
    
    STATUS("Mounting FatFS");
    
    FRESULT status = f_mount(0, &fatfs);
    
    if (status == FR_OK)
        STATUS("OK");
    else
        STATUS("FatFS FAIL");
}

/* Bios patches */

typedef int (*func_t)();

// __GetDev_SN() is called by SYS as part of the bootup sequence.
// We patch it to initialize AlterBIOS.
int patch_GetDev_SN()
{
    // Only when called during boot, as detected by GPIO config status
    if (GPIOA->CRH == 0x88833BBB)
    {
        alterbios_init();
    }
    
    // Return the real serial number
    return ((func_t)*(uint32_t*)0x08000020)();
}


