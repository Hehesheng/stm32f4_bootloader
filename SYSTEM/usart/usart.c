#include "sys.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //ucos 使用
#endif

//串口中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15,   接收完成标志
//bit14~8, reserved
//bit7~0,  接收到的有效字节数目
u16 USART_RX_STA = 0; //接收状态标记

/*
 * To implement the STDIO functions you need to create
 * the _read and _write functions and hook them to the
 * USART you are using. This example also has a buffered
 * read function for basic line editing.
 */
int _write(int fd, char *ptr, int len);
int _read(int fd, char *ptr, int len);
void get_buffered_line(void);

/*
 * Called by libc stdio fwrite functions
 */
int _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
     * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2)
    {
        return -1;
    }

    while (*ptr && (i < len))
    {
        USART_SendData(USART1, *ptr);

        if (*ptr == '\n')
        {
            USART_SendData(USART1, '\r');
        }

        i++;
        ptr++;
    }

    return i;
}

/* back up the cursor one space */
static inline void back_up(void)
{
#if scanfSupport
    end_ndx = dec_ndx(end_ndx);
#endif
    USART_SendData(USART1, '\010');
    USART_SendData(USART1, ' ');
    USART_SendData(USART1, '\010');
}

#if scanfSupport
/*
 * This is a pretty classic ring buffer for characters
 */
#define BUFLEN 127
static char buf[BUFLEN + 1];
static uint16_t start_ndx;
static uint16_t end_ndx;
#define buf_len ((end_ndx - start_ndx) % BUFLEN)
static inline int inc_ndx(int n)
{
    return ((n + 1) % BUFLEN);
}
static inline int dec_ndx(int n)
{
    return (((n + BUFLEN) - 1) % BUFLEN);
}

/*
 * A buffered line editing function.
 */
void get_buffered_line(void)
{
    char c;

    if (start_ndx != end_ndx)
    {
        return;
    }
    while (1)
    {
        while (!(USART1->SR & USART_FLAG_RXNE))
            ;
        c = USART_ReceiveData(USART1);

        if (c == '\r')
        {
            buf[end_ndx] = '\n';
            end_ndx = inc_ndx(end_ndx);
            buf[end_ndx] = '\0';
            USART_SendData(USART1, '\r');
            USART_SendData(USART1, '\n');
            return;
        }
        /* or DEL erase a character */
        if ((c == '\010') || (c == '\177'))
        {
            if (buf_len == 0)
            {
                USART_SendData(USART1, '\a');
            }
            else
            {
                back_up();
            }
            /* erases a word */
        }
        else if (c == 0x17)
        {
            while ((buf_len > 0) &&
                   (!(isspace((int)buf[end_ndx]))))
            {
                back_up();
            }
            /* erases the line */
        }
        else if (c == 0x15)
        {
            while (buf_len > 0)
            {
                back_up();
            }
            /* Non-editing character so insert it */
        }
        else
        {
            if (buf_len == (BUFLEN - 1))
            {
                USART_SendData(USART1, '\a');
            }
            else
            {
                buf[end_ndx] = c;
                end_ndx = inc_ndx(end_ndx);
                USART_SendData(USART1, c);
            }
        }
    }
}

/*
 * Called by the libc stdio fread fucntions
 *
 * Implements a buffered read with line editing.
 */
int _read(int fd, char *ptr, int len)
{
    int my_len;

    if (fd > 2)
    {
        return -1;
    }
    get_buffered_line();
    my_len = 0;

    while ((buf_len > 0) && (len > 0))
    {
        *ptr++ = buf[start_ndx];
        start_ndx = inc_ndx(start_ndx);
        my_len++;
        len--;
    }

    return my_len; /* return the length we got */
}
#endif

//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //使能USART1时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1

    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化PA9，PA10

    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART1, &USART_InitStructure);                                       //初始化串口1

    USART_Cmd(USART1, ENABLE); //使能串口1

    //USART_ClearFlag(USART1, USART_FLAG_TC);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          //串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; //抢占优先级13
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                            //根据指定的参数初始化VIC寄存器
}

void USART1_IRQHandler(void) //串口1中断服务程序
{
    u8 Res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART1); //(USART1->DR);	//读取接收到的数据

        if ((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (Res == 0x0d || Res == 0x0a) //接收到了回车
            {
                USART_RX_STA |= 0x8000;
            }
            else
            {
                /* or DEL erase a character */
                if ((Res == '\010') || (Res == '\177'))
                {
                    if ((USART_RX_STA & 0X3FFF) == 0)
                    {
                        USART_SendData(USART1, '\a');
                    }
                    else
                    {
                        back_up();
                        USART_RX_BUF[(USART_RX_STA & 0X7FFF) - 1] = 0;
                        USART_RX_STA--;
                    }
                    /* erases a word */
                }
                else //正常接收数据
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_SendData(USART1, Res);
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //数据出错, 重新接收
                }
            }
        }
    }
}
