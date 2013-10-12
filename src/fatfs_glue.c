/* Glue for FatFS accessing the DSO Quad internal disc.
 */

#include "fatfs/diskio.h"
#include "DS203/BIOS.h"
#include "gpio.h"
#include <stdbool.h>

typedef enum {
    FLASH_M25PE16 = 0,
    FLASH_8M = 1 /* Winbond 25q64bvsig */
} flash_type_t;

flash_type_t g_flash_type = FLASH_M25PE16;
bool g_invert = true;

/* Flash chip abstraction */
static int sectorlen()
{
    if (g_flash_type == FLASH_M25PE16)
        return 512;
    else if (g_flash_type == FLASH_8M)
        return 4096;
    else
        return 512; /* Fallback */
}

static int sectorcount()
{
    if (g_flash_type == FLASH_M25PE16)
        return 4096;
    else if (g_flash_type == FLASH_8M)
        return 2048;
    else
        return 0; /* Fallback */
}

/* SPI access routines */
DECLARE_GPIO(cs, GPIOB, 7);

static void spi_send(uint8_t byte)
{
    while (!(SPI3->SR & SPI_SR_TXE));
    SPI3->DR = byte;
}

static uint8_t spi_recv()
{
    while (!(SPI3->SR & SPI_SR_TXE));
    while (SPI3->SR & SPI_SR_BSY); // Wait for previous byte transmission
    always_read(SPI3->DR); // Clear RXNE & overflow flags
    always_read(SPI3->SR);
    SPI3->DR = 0xFF;
    while (!(SPI3->SR & SPI_SR_RXNE));
    return SPI3->DR;
}

static void  __attribute__((optimize("O2")))
spi_recv_block(uint8_t *buffer, unsigned count)
{
    while (!(SPI3->SR & SPI_SR_TXE));
    while (SPI3->SR & SPI_SR_BSY); // Wait for previous byte transmission
    always_read(SPI3->DR); // Clear RXNE & overflow flags
    always_read(SPI3->SR);
    
    SPI3->DR = 0xFF;
    while (count--)
    {
        while (!(SPI3->SR & SPI_SR_TXE));
        
        __disable_irq(); // Interrupt in here could cause RX overflow
        SPI3->DR = 0xFF;
        while (!(SPI3->SR & SPI_SR_RXNE));
        
        if (g_invert)
            *buffer++ = SPI3->DR ^ 0xFF; // SYS prior to 1.60 stores the inverse of bytes
        else
            *buffer++ = SPI3->DR;
       
        __enable_irq();
    }
}

/* Flash (M25PE16) access routines (faster than the BIOS versions) */

#define WREN       0x06  // Write enable instruction 
#define READ       0x03  // Read from Memory instruction 
#define RDSR       0x05  // Read Status Register instruction  
#define PP         0x02  // Write to Memory instruction 
#define PE         0xDB  // Page Erase instruction 
#define PW         0x0A  // Page write instruction 
#define DP         0xB9  // Deep power-down instruction 
#define RDP        0xAB  // Release from deep power-down instruction 
#define WIP_Flag   0x01  // Write In Progress (WIP) flag 

static void wait_for_write_end()
{
    gpio_cs_set(0);
    spi_send(RDSR);
    while (spi_recv() & WIP_Flag);
    gpio_cs_set(1);
}

static void read_flash(uint8_t *buffer, unsigned addr, unsigned count)
{
    // USB accesses may disturb our reads. We detect this by checking
    // CS after transfer.
    do {
        gpio_cs_set(1);
        wait_for_write_end();
        
        gpio_cs_set(0);
        
        spi_send(READ);
        spi_send((addr >> 16) & 0xFF);
        spi_send((addr >> 8) & 0xFF);
        spi_send(addr & 0xFF);
    
        spi_recv_block(buffer, count);
    
        spi_recv();
    } while (gpio_cs_get());
    
    gpio_cs_set(1);
}

/* FatFS hook implementations */

DSTATUS disk_initialize(BYTE drv)
{
    if (drv != 0) return STA_NOINIT;

    /* Check the flash type */
    /* Code adapted from SYS_V1.60 */
    uint8_t *ver = (uint8_t*)__Get(DFUVER);
    int v = (ver[1] - '0') * 100 + (ver[3] - '0') * 10 + (ver[4] - '0');
    
    if (v <= 311)
    {
        g_flash_type = FLASH_M25PE16;
    }
    else
    {
        uint8_t *info = (uint8_t*)__Get(DEVICEINFO);
        
        if (info[0] == '8' && info[1] == 'M' && info[2] == 'B')
            g_flash_type = FLASH_8M;
        else
            g_flash_type = FLASH_M25PE16;    
    }
    
    /* Check flash inversion */
    uint8_t buf[8];
    read_flash(buf, 0, sizeof(buf));
    if (buf[0] == 0x14 && buf[1] == 0xc3)
        g_invert = false;
    
    return 0;
}

DSTATUS disk_status (BYTE drv)
{
    return (drv == 0) ? 0 : STA_NOINIT;
}

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
    if (drv != 0 || count == 0) return RES_PARERR;
    
    int s = sectorlen();
    read_flash(buff, sector * s, count * s);
    
    return RES_OK;
}

DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
    if (drv != 0 || count == 0) return RES_PARERR;
    
    while (count--)
    {
        if (g_flash_type == FLASH_M25PE16)
        {
            if (__ProgDiskPage((u8*)buff, sector * 512) != 0 ||
                __ProgDiskPage((u8*)buff + 256, sector * 512 + 256) != 0)
                return RES_ERROR;
            
            sector++;
            buff += 512;
        }
        else if (g_flash_type == FLASH_8M)
        {
            if (__ProgDiskPage((u8*)buff, sector * 4096) != 0)
                return RES_ERROR;
            
            sector++;
            buff += 4096;
        }
    }
    
    return RES_OK;
}

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
    if (drv != 0) return RES_PARERR;
    
    if (ctrl == CTRL_SYNC)
    {
        return RES_OK;
    }
    else if (ctrl == GET_SECTOR_COUNT)
    {
        *(DWORD*)buff = sectorcount();
        return RES_OK;
    }
    else if (ctrl == GET_SECTOR_SIZE || ctrl == GET_BLOCK_SIZE)
    {
        *(DWORD*)buff = sectorlen();
        return RES_OK;
    }
    else
    {
        return RES_PARERR;
    }
}

DWORD get_fattime (void)
{
        /* Pack date and time into a DWORD variable */
        return    ((DWORD)(2000 - 1980) << 25)
                        | ((DWORD)1 << 21)
                        | ((DWORD)1 << 16)
                        | ((DWORD)13 << 11)
                        | ((DWORD)37 << 5)
                        | ((DWORD)0 >> 1);
}
