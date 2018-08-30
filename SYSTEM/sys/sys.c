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
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, DISABLE);
    NVIC_SetVectorTable(FLASH_BASE, ApplicationAddress^FLASH_BASE);
    __enable_irq();
}
