#include "myFatFs.h"

#if FF_FS_MINIMIZE <= 1
/**
  * @brief 获取目录下所有文件, 并串口输出
  *
  * @param [IN]path:文件目录
  *
  * @retval 获取是否成功
  */
FRESULT scan_files(const TCHAR *path)
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
                break;                    //错误了/到末尾了,退出
            printf("%s/", path);          //打印目录
            printf("%s", fileinfo.fname); //打印文件
            printf("\t%10lu KB\t0X%X\n", fileinfo.fsize >> 10, fileinfo.fattrib);
        }
    }
    f_closedir(&dir);
    return res;
}

/**
  * @brief 寻找文件目录下是否有Bin文件
  *
  * @param [IN]path:文件目录    [OUT]binName:如果找到了, 返回文件名, 否则为NULL
  *
  * @note binName的长度根据FF_LFN_BUF调节, 请保证binName放得下文件全名
  *
  * @retval 是否有Bin(0:没有, 1:有)
  */
u8 findBin(const TCHAR *path, TCHAR *binName)
{
    DIR dir;
    FRESULT res;
    FILINFO fileinfo;

    res = f_opendir(&dir, (const TCHAR *)path); //打开一个目录
    if (res == FR_OK)
    {
        while (1)
        {
            res = f_readdir(&dir, &fileinfo); //读取目录下的一个文件
            if (res != FR_OK || fileinfo.fname[0] == 0)
                break;                     //错误了/到末尾了,退出
            if (fileinfo.fattrib & AM_ARC) //如果读到这是一个文件
            {
                if ((!strcmp(fileinfo.fname + (strlen(fileinfo.fname) - 4), ".bin") |
                     !strcmp(fileinfo.fname + (strlen(fileinfo.fname) - 4), ".BIN")))
                {
                    strcpy(binName, fileinfo.fname);
                    f_closedir(&dir);
                    return 1; //成功找到文件
                }
            }
        }
    }
    f_closedir(&dir);
    return 0;
}
#endif

#if !FF_FS_READONLY && !F_FS_MINIMIZE
/**
  * @brief 获取SD卡的剩余空间
  *
  * @param [IN]fs:挂载目录  [IN]path:文件目录   [OUT]tot:总容量 [OUT]fre:剩余容量
  *
  * @retval 获取是否成功
  */
FRESULT getSDcardSpace(FATFS fs, const TCHAR *path, DWORD *tot, DWORD *fre)
{
    FRESULT res;
    DWORD fre_clust;
    /* Get volume information and free clusters of drive */
    res = f_getfree(path, &fre_clust, &fs);
    if (res)
    {
        printf("Get Free Fail\n");
        return res;
    }
    else
    {
        /* Get total sectors and free sectors */
        *tot = ((fs.n_fatent - 2) * fs.csize) >> 1;
        *fre = (fre_clust * fs.csize) >> 1;
        return res;
    }
}
#endif
