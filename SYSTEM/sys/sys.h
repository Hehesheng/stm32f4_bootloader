#ifndef __SYS_H
#define __SYS_H
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
#include "ff.h"

//0,不支持os
//1,支持os
#define SYSTEM_SUPPORT_OS 0 //定义系统文件夹是否支持OS

//IAP Application Address
#define BootloaderAddress  0x08000000
#define ApplicationAddress 0x08010000

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 20) //0x40020014
#define GPIOB_ODR_Addr (GPIOB_BASE + 20) //0x40020414
#define GPIOC_ODR_Addr (GPIOC_BASE + 20) //0x40020814
#define GPIOD_ODR_Addr (GPIOD_BASE + 20) //0x40020C14
#define GPIOE_ODR_Addr (GPIOE_BASE + 20) //0x40021014
#define GPIOF_ODR_Addr (GPIOF_BASE + 20) //0x40021414
#define GPIOG_ODR_Addr (GPIOG_BASE + 20) //0x40021814
#define GPIOH_ODR_Addr (GPIOH_BASE + 20) //0x40021C14
#define GPIOI_ODR_Addr (GPIOI_BASE + 20) //0x40022014

#define GPIOA_IDR_Addr (GPIOA_BASE + 16) //0x40020010
#define GPIOB_IDR_Addr (GPIOB_BASE + 16) //0x40020410
#define GPIOC_IDR_Addr (GPIOC_BASE + 16) //0x40020810
#define GPIOD_IDR_Addr (GPIOD_BASE + 16) //0x40020C10
#define GPIOE_IDR_Addr (GPIOE_BASE + 16) //0x40021010
#define GPIOF_IDR_Addr (GPIOF_BASE + 16) //0x40021410
#define GPIOG_IDR_Addr (GPIOG_BASE + 16) //0x40021810
#define GPIOH_IDR_Addr (GPIOH_BASE + 16) //0x40021C10
#define GPIOI_IDR_Addr (GPIOI_BASE + 16) //0x40022010

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) //输出
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  //输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) //输出
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  //输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) //输出
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  //输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) //输出
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  //输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) //输出
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) //输出
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) //输出
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr, n) //输出
#define PHin(n) BIT_ADDR(GPIOH_IDR_Addr, n)  //输入

#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr, n) //输出
#define PIin(n) BIT_ADDR(GPIOI_IDR_Addr, n)  //输入

#define ADDR_FLASH_SECTOR_0 ((u32)0x08000000)  //扇区0起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_1 ((u32)0x08004000)  //扇区1起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_2 ((u32)0x08008000)  //扇区2起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_3 ((u32)0x0800C000)  //扇区3起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_4 ((u32)0x08010000)  //扇区4起始地址, 64 Kbytes
#define ADDR_FLASH_SECTOR_5 ((u32)0x08020000)  //扇区5起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_6 ((u32)0x08040000)  //扇区6起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_7 ((u32)0x08060000)  //扇区7起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_8 ((u32)0x08080000)  //扇区8起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_9 ((u32)0x080A0000)  //扇区9起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_10 ((u32)0x080C0000) //扇区10起始地址,128 Kbytes
#define ADDR_FLASH_SECTOR_11 ((u32)0x080E0000) //扇区11起始地址,128 Kbytes

//IAP Function
void jumpToApp(void);
void beforeAppBegin(void);
FRESULT updateApplication(const TCHAR *path);

#endif
