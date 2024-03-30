
/* 初始化UART0 */
void Uart_Init()
{
    led_test(); 
    /* 设置引脚为串口模式(GPH用于TXD0、RXD0) */
    volatile unsigned int *pGPHCON = (volatile unsigned int *)0x56000070;
    *pGPHCON |= (1 << 5);
    *pGPHCON |= (1 << 7);
    *pGPHCON &= ~(1 << 4);
    *pGPHCON &= ~(1 << 6);

    /* 上拉管脚 */
    volatile unsigned int *pGPHUP = (volatile unsigned int *)0x56000078;
    *pGPHUP &= ~((1 << 2)| (1 << 3));
    
    /* 设置波特率（115200） */
    /* UBRDIVn = (int)(PCLK/(bps x 16) ) -1 */
    /* UBRDIVn = 50000000 /（115200*16）-1 = 26 */
    volatile unsigned int *pUCON0 = (volatile unsigned int *)0x50000004;
    *pUCON0 |= 5;   /* 时钟源设置成pclock（50M）；中断/查询模式；使能收/发 */
    volatile unsigned int *pUBRDIV0 = (volatile unsigned int *)0x50000028;
    *pUBRDIV0 = 26;  /* 设置波特率为115200 */

    /* 设置数据格式 */
    volatile unsigned int *pULCON0 = (volatile unsigned int *)0x50000000;
    *pULCON0 = 3;   /* 8n1:8个数据位，无校验位，1个停止位 */
    puts("Uart_Init ... \r\n");

    return;
}

/* 发送一个byte */
void putchar(volatile unsigned char c)
{
    volatile unsigned int *pUTRSTAT0 = (volatile unsigned int *)0x50000010;
    volatile unsigned char *pUTXH0 = (volatile unsigned char *)0x50000020;

    while (!(*pUTRSTAT0 & (1 << 2)));  /* 等待上一个数据发送完成后退出while */
    *pUTXH0 = c;
    return;
}

/* 发送多个byte */
void puts(unsigned char *s)
{
    while (*s) {
        putchar(*s);
        ++s;
    }
    return;
}

/* 接收一个byte */
volatile unsigned char getchar(void)
{    
    volatile unsigned int *pUTRSTAT0 = (volatile unsigned int *)0x50000010;
    volatile unsigned char *pURXH0 = (volatile unsigned char *)0x50000024;

    while (!(*pUTRSTAT0 & 1));  /* 接收到数据退出while */
    return *pURXH0;
}

/* 打印变量32位变量a的值 */
void pri(char* s, int a)  //0x00000800
{
    char tmp;
    int i = 7;
    
    puts(s);
    for (; i >= 0; --i) {
        tmp = (a >> (i * 4)) & 0xf;
        if (tmp < 10) putchar(tmp + '0');
        else putchar(tmp + 'a' - 10);
    }
    
    //puts("\r\n");
    return;
}

/* 打印变量8位变量a的值 */
void pri_byte(char* s, char a)  //0x85
{
    char tmp;
    int i = 1;
    
    puts(s);
    for (; i >= 0; --i) {
        tmp = (a >> (i * 4)) & 0xf;
        if (tmp < 10) putchar(tmp + '0');
        else putchar(tmp + 'a' - 10);
    }
    
    puts("\r\n");
    return;
}


