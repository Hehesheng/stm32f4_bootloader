#include "main.h"

int main()
{
    char binName[256];
    FATFS FatFs;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置中断
    delay_init(168);                                //delay初始化，系统主频168MHz
    uart_init(115200);                              //USART1波特率设置为115200

    TIM3_PWM_Init(2000 - 1, 16800 - 1); // 84M/(8400*2000) = 5hz
    TIM_SetCompare1(TIM3, 1000);        // 比较值CCR1为1000

    if (f_mount(&FatFs, "0:", 1) == FR_OK)
    {
        if (findBin("0:", binName + 2))
        {
            binName[0] = '0';
            binName[1] = ':';
            printf("Find file: %s\r\n", binName);
            printf("Updata Begin!!!\r\n");
            if (updateApplication(binName) != FR_OK)
                printf("Updata Fail!!\r\n");
            printf("\n\rCompleted!\r\n");
        }
        else
            printf("Not Found Bin File\r\n");
    }
    f_unmount("0:");

    jumpToApp();

    while (1)
        ;
}
