#include "myFatFs.h"

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
                break;                        //错误了/到末尾了,退出
            printf("%s/", path);              //打印目录
            printf("%s", fileinfo.fname); //打印文件
            printf("\t%10lu KB\t0X%X\n", fileinfo.fsize >> 10, fileinfo.fattrib);
            printf("%10X %10X\n", fileinfo.fdate, fileinfo.ftime);
        }
    }
    f_closedir(&dir);
    return res;
}

