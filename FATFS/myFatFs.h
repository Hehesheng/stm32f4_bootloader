#ifndef __MYFATFS_H
#define __MYFATFS_H
#include "ff.h"
#include "usart.h"
#include "string.h"

FRESULT scan_files(const TCHAR *path);
FRESULT getSDcardSpace(FATFS *fs, const TCHAR *path, DWORD *tot, DWORD *fre);
u8 findBin(const TCHAR *path, TCHAR *binName);

#endif //__MYFATFS_H
