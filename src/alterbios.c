#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "DS203/BIOS.h"
#include "stm32f10x.h"

#define VERSION "AlterBIOS " COMMITID

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
    // We want to initialize exactly once after boot.
    // Usually this is accomplished by hooking the GetDev_SN from the BIOS.
    // However, if normal SYS is written over AlterBIOS, our BIOS hooks are
    // gone. To maintain functionality of AlterBIOS-based programs, they
    // should call this function also.
    //
    // To avoid double-initialization, we write a dummy value to a peripheral
    // register. These registers are always cleared when system reset occurs,
    // unlike RAM.
    if (CRC->IDR == 0xA1)
    {
        return;
    }
    
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    CRC->IDR = 0xA1;
    
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

static void fix_filename(const u8 *orig, char dest[14])
{
    char *p = dest;
    int i;
    
    for (i = 0; i < 8 && (orig[i] != ' '); i++)
    {
        *p++ = orig[i];
    }
    
    *p++ = '.';
    
    for (i = 8; i < 11; i++)
    {
        *p++ = orig[i];
    }
    
    *p++ = 0;
}

// Error code conversion for BIOS API
u8 ERR(FRESULT r)
{
    if (r == FR_OK)
        return 0; // OK
    else if (r == FR_DISK_ERR)
        return 1; // SEC_ERR
    else if (r == FR_INT_ERR)
        return 2; // FAT_ERR
    else if (r == FR_NO_FILE || r == FR_NO_PATH)
        return 4; // NEW
    else
        return 2; // FAT_ERR
}

// A single file entry for BIOS API emulation
FIL g_file;

u8 patch_OpenFileWr(u8 *buffer, u8 *filename, u16 *cluster, u32 *diraddr)
{
    char fname[14];
    fix_filename(filename, fname);
    
    FRESULT r = f_open(&g_file, fname, FA_WRITE | FA_CREATE_ALWAYS);
    *cluster = g_file.sclust;
    return ERR(r);
}

u8 patch_OpenFileRd(u8 *buffer, u8 *filename, u16 *cluster, u32 *diraddr)
{
    char fname[14];
    fix_filename(filename, fname);
    
    FRESULT r = f_open(&g_file, fname, FA_READ | FA_OPEN_EXISTING);
    *cluster = g_file.sclust;
    return ERR(r);
}

DWORD clust2sect(FATFS *fs, DWORD clst);

u8 patch_ReadFileSec(u8* buffer, u16* cluster)
{
    if (*cluster != g_file.sclust)
    {
        // APP PLUS V1.06 does crazy stuff when backuping
        // the parameter file. This is a workaround for it.
        disk_read(0, buffer, clust2sect(&fatfs, *cluster), 1);
        return 0;
    }
    
    unsigned rcount;
    FRESULT r = f_read(&g_file, buffer, 512, &rcount);
    
    return ERR(r);
}

u8 patch_ProgFileSec(u8* buffer, u16* cluster)
{
    unsigned wcount;
    FRESULT r = f_write(&g_file, buffer, 512, &wcount);
    
    if (wcount != 512)
        return 3; // Disk is full
    
    return ERR(r);
}

u8 patch_CloseFile(u8* buffer, u32 length, u16* cluster, u32* diraddr)
{
    FRESULT r = f_lseek(&g_file, length);
    if (r != FR_OK)
        return ERR(r);
    
    r = f_close(&g_file);
    return ERR(r);
}

