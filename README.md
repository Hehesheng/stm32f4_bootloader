## Bootloader

### V1.1.2
大版本更新, 从SD卡的根目录寻找第一个读取到的`.bin`文件进行升级, 加强了稳定性, 且有串口显示提醒:

显示找到的文件名, 并以 **"#"** 作为提示符, 每个 **"#"** 表示成功刷入2k文件

### V1.1.0
可以从SD卡直接读取`.bin`文件进行升级

### V1.0.5
加入了`FATFS`文件系统支持

### V1.0.0
制作的stm32f4的BootLoader

GPIOA_6会输出PWM, 表示正常运行

在boot中已经重置了时钟, 只需要修改`ApplicationAddress`和App程序中的Flash开始地址就可以, 且需在App程序中添加语句, FreeRTOS运行验证.

### 1.Introduction

使用stm32f407核心板, 偏移64k的flash空间, 虽然代码本体只有32kb不到, 而且FatFs开启了读写模式, 而实际使用中只用到了读, 所以还有很多压缩空间

### 2.Build

1.克隆库到本地

```shell
git clone git@github.com:Hehesheng/stm32f4_bootloader.git
cd ~/stm32f4_bootloader
```

2.修改Makefile

你需要[GNU Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)选择对应的版本下载, 然后将可执行文件路径写入`Makefile`第54中, 例如:

```makefile
BINPATH = /user/name/arm-toolchain/bin
```

3.编译

在命令行输入:

```:hong_kong:
make all
```

就能在`OBJ`文件夹中找到`Output.elf`以及`Output.bin`, 将他们下载到板子中就好了.

