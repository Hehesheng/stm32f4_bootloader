## Bootloader

### V1.1.0
可以从SD卡直接读取`.bin`文件进行升级

### V1.0.5
加入了`FATFS`文件系统支持

### V1.0.0
制作的stm32f4的BootLoader

GPIOA_6会输出PWM, 表示正常运行

在boot中已经重置了时钟, 只需要修改`ApplicationAddress`和App程序中的Flash开始地址就可以, 且需在App程序中添加语句, FreeRTOS运行验证.
