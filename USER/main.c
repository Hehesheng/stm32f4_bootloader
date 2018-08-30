#include "main.h"

FRESULT scan_files(char *path);

int main()
{
    FATFS FatFs;
    FRESULT res;
    DWORD fre_clust, fre_sect, tot_sect;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置中断
    delay_init(168);                                //delay初始化，系统主频168MHz
    uart_init(115200);                              //USART1波特率设置为115200

    TIM3_PWM_Init(2000 - 1, 16800 - 1); // 84M/(8400*2000) = 5hz
    TIM_SetCompare1(TIM3, 1000);        // 比较值CCR1为500

    f_mount(&FatFs, "0:", 1);

    scan_files("0:");

    delay_ms(3000);

    /* Get volume information and free clusters of drive 1 */
    res = f_getfree("0:", &fre_clust, &FatFs);
    if (res)
    {
        printf("Get Free Fail\n");
    }
    else
    {
        /* Get total sectors and free sectors */
        tot_sect = (FatFs.n_fatent - 2) * FatFs.csize;
        fre_sect = fre_clust * FatFs.csize;
        /* Print the free space (assuming 512 bytes/sector) */
        printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
    }

    f_unmount("0:");

    jumpToApp();
    while (1)
        ;
}

FRESULT scan_files(char *path) /* Start node to be scanned (***also used as work area***) */
{
    DIR dir;
    FRESULT res;
    FILINFO fileinfo;

    res = f_opendir(&dir, (const TCHAR *)path); //打开一个目录
    if (res == FR_OK)
    {
        printf("\r\n");
        while (1)
        {
            res = f_readdir(&dir, &fileinfo); //读取目录下的一个文件
            if (res != FR_OK || fileinfo.fname[0] == 0)
                break;                  //错误了/到末尾了,退出
            printf("%s/", path);        //打印目录
            printf("%s\r\n", fileinfo.fname); //打印文件夹
        }
    }
    f_closedir(&dir);
    return res;
}

// FRESULT scan_files(char *path) /* Start node to be scanned (***also used as work area***) */
// {
//     FRESULT res;
//     DIR dir;
//     UINT i;
//     static FILINFO fno;

//     res = f_opendir(&dir, path); /* Open the directory */
//     if (res == FR_OK)
//     {
//         for (;;)
//         {
//             res = f_readdir(&dir, &fno); /* Read a directory item */
//             if (res != FR_OK || fno.fname[0] == 0)
//                 break; /* Break on error or end of dir */
//             if (fno.fattrib & AM_DIR)
//             { /* It is a directory */
//                 i = strlen(path);
//                 sprintf(&path[i], "/%s", fno.fname);
//                 res = scan_files(path); /* Enter the directory */
//                 if (res != FR_OK)
//                     break;
//                 path[i] = 0;
//             }
//             else
//             { /* It is a file. */
//                 printf("%s/%s\n", path, fno.fname);
//             }
//         }
//         f_closedir(&dir);
//     }
//     return res;
// }