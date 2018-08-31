#include "sys.h"

/**
  * @brief 从 Boot 跳转到 App
  *
  * @note  跳转地址在 HeadFile 里设置 ApplicationAddress
  */
void jumpToApp(void)
{
    unsigned int JumpAddress;
    typedef void (*pFunction)(void);
    pFunction Jump_To_Application;
    //程序的第二个字存放的是复位函数的地址，
    //通过这个地址跳转的应用程序中
    JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);

    Jump_To_Application = (pFunction)JumpAddress;

    //指向应用程序的栈顶
    __set_MSP(*(__IO uint32_t *)ApplicationAddress);

    //执行应用程序的复位函数
    beforeJumpToApp();
    Jump_To_Application();
}

/**
  * @brief 调用jump_To_Application之前要调用这个函数, 会重置中断, 时钟, 设置中断向量表
  */
void beforeJumpToApp(void)
{
    __disable_irq();
    //Reset All Enable
    RCC->AHB1RSTR |= 0XFFFFFFFF;
    RCC->AHB2RSTR |= 0XFFFFFFFF;
    RCC->AHB3RSTR |= 0XFFFFFFFF;
    RCC->APB1RSTR |= 0XFFFFFFFF;
    RCC->APB2RSTR |= 0XFFFFFFFF;
    //Reset All Disable
    RCC->AHB1RSTR &= ~0XFFFFFFFF;
    RCC->AHB2RSTR &= ~0XFFFFFFFF;
    RCC->AHB3RSTR &= ~0XFFFFFFFF;
    RCC->APB1RSTR &= ~0XFFFFFFFF;
    RCC->APB2RSTR &= ~0XFFFFFFFF;
    __enable_irq();
}

/**
  * @brief 往Flash中写入数据
  *
  * @param [IN]beginAddress:开始地址    [IN]buff:输入数据   [IN]size:数据大小
  *
  * @retval 是否写入成功
  */
FLASH_Status flashWrite(uint32_t beginAddress, uint32_t *buff, uint32_t size)
{
    u32 addrx = 0, endaddr = 0;
    FLASH_Status status = FLASH_COMPLETE;
    FLASH_Unlock();
    FLASH_DataCacheCmd(DISABLE);
    addrx = beginAddress;          //写入的起始地址
    endaddr = beginAddress + size; //写入的结束地址
    if (addrx < 0X1FFF0000)        //只有主存储区,才需要执行擦除操作!!
    {
        while (addrx < endaddr) //扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
        {
            if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF) //有非0XFFFFFFFF的地方,要擦除这个扇区
            {
                if (FLASH_EraseSector(STMFLASH_GetFlashSector(addrx), VoltageRange_3) != FLASH_COMPLETE)
                    return FLASH_BUSY; //发生错误了
            }
            else
                addrx += 4;
        }
    }
    if (status == FLASH_COMPLETE)
    {
        while (beginAddress < endaddr) //写数据
        {
            if (FLASH_ProgramWord(beginAddress, *buff) != FLASH_COMPLETE) //写入数据
            {
                return FLASH_BUSY; //写入异常
            }
            beginAddress += 4;
            buff ++;
        }
    }
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
    return FLASH_COMPLETE;
}

uint16_t STMFLASH_GetFlashSector(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_Sector_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_Sector_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_Sector_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_Sector_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_Sector_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_Sector_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_Sector_6;
    else if (addr < ADDR_FLASH_SECTOR_8)
        return FLASH_Sector_7;
    else if (addr < ADDR_FLASH_SECTOR_9)
        return FLASH_Sector_8;
    else if (addr < ADDR_FLASH_SECTOR_10)
        return FLASH_Sector_9;
    else if (addr < ADDR_FLASH_SECTOR_11)
        return FLASH_Sector_10;
    return FLASH_Sector_11;
}

inline u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32 *)faddr;
}
