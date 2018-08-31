#include "main.h"

int main()
{
    char binName[256];
    u8 val;
    FATFS FatFs;
    FRESULT res;
    DWORD tot, fre;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置中断
    delay_init(168);                                //delay初始化，系统主频168MHz
    uart_init(115200);                              //USART1波特率设置为115200

    TIM3_PWM_Init(2000 - 1, 16800 - 1); // 84M/(8400*2000) = 5hz
    TIM_SetCompare1(TIM3, 1000);        // 比较值CCR1为500

    f_mount(&FatFs, "0:", 1);

    delay_ms(3000);
    val = findBin("0:", binName);
    if (val)
    {
        printf("File Name: 0:/%s\n", binName);
    }
    else
        printf("Not Found Bin File\n");

    f_unmount("0:");

    jumpToApp();

    while (1)
        ;
}

