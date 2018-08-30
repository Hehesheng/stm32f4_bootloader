/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* FatFs lower layer API */
#include "sdio_sdcard.h"

/* Definitions of physical drive number for each drive */
#define SD_CARD 0 //SD_CARD

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    u8 result;

    switch (pdrv)
    {
    case SD_CARD:
        result = SD_Init();
        break;
    default:
        result = 1;
    }
    if (result)
        return STA_NOINIT;
    else
        return 0; //初始化成功
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    DWORD sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
)
{
    u8 res = 0;
    if (!count)
        return RES_PARERR; //count不能等于0，否则返回参数错误
    switch (pdrv)
    {
    case SD_CARD: //SD卡
        res = SD_ReadDisk(buff, sector, count);
        while (res) //读出错
        {
            SD_Init(); //重新初始化SD卡
            res = SD_ReadDisk(buff, sector, count);
        }
        break;
    default:
        res = 1;
    }
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if (res == 0x00)
        return RES_OK;
    else
        return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    DWORD sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
)
{
    u8 res = 0;
    if (!count)
        return RES_PARERR; //count不能等于0，否则返回参数错误
    switch (pdrv)
    {
    case SD_CARD: //SD卡
        res = SD_WriteDisk((u8 *)buff, sector, count);
        while (res) //写出错
        {
            SD_Init(); //重新初始化SD卡
            res = SD_WriteDisk((u8 *)buff, sector, count);
            //printf("sd wr error:%d\r\n",res);
        }
        break;
    default:
        res = 1;
    }
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if (res == 0x00)
        return RES_OK;
    else
        return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
    DRESULT res;
    if (pdrv == SD_CARD) //SD卡
    {
        switch (cmd)
        {
        case CTRL_SYNC:
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(DWORD *)buff = 512;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(WORD *)buff = SDCardInfo.CardBlockSize;
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SDCardInfo.CardCapacity / 512;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
            break;
        }
    }
    else
        res = RES_ERROR; //其他的不支持
    return res;
}
