#include "s3c2440_soc.h"
#include "uart.h"
#include "stl.h"

static unsigned int *bss_end = NULL;

/* 获取bss end addr */
unsigned int *get_bss_end()
{
    return bss_end;
}

/* 初始化sdram */
void sdram_init()
{
    BWSCON = 0x22000000;
    BANKCON6 = 0x18001;
    BANKCON7 = 0x18001;
    REFRESH = 0x8404f5;
    BANKSIZE = 0xb1;
    MRSRB6 = 0x20;
    MRSRB7 = 0x20;
    return;
}

/* sdram读写测试 
    从p开始往后读/写n个byte */
int sdram_test(volatile unsigned char *p, unsigned int n)
{
    int i = 0;

    /* 写SDRAM */
    for (i = 0; i < n; ++i) {
        p[i] = 0x55;
    }

    /* 读sdram */
    for (i = 0; i < n; ++i) {
        if (p[i] != 0x55) {
            puts("read sdram fail \r\n");
            return 1;
        }
    }
    
    puts("read sdram success \r\n");

    return 0;
}

/* 判断是否为nor启动 */
int isBootFromNorFlash() {
    volatile unsigned int *p = (volatile unsigned int *)0;
    unsigned int val = *p;

    *p = 0x12345678;
    if (*p == 0x12345678) {
        *p = val;
        return 0;
    } else {
        return 1;
    }
}

/* 重定位text、rodata、data段 */
void copy2sdram(void) {
    /* 从lds中获取__code_start、__bss_start，再从0地址把数据复制到__code_start~__bss_start */
    extern int __code_start, __bss_start;
    volatile unsigned int *src = (volatile unsigned int *)0;
    volatile unsigned int *dst = (volatile unsigned int *)&__code_start;
    volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
    unsigned int len = (unsigned int)&__bss_start - (unsigned int)&__code_start;
    
    Uart_Init();
    puts("Uart_Init \r\n");
    if (isBootFromNorFlash()) {
        /* nor启动 */
        while (dst < end) {
            *dst = *src;
            ++dst;
            ++src;
        }
    } else {
        /* nand启动 */
        nand_init();
        nand_read(src, dst, len);
    }
    puts("copy2sdram \r\n");
    return;
}

/* 清除bss段 */
void clean_bss(void) {
    extern int _end, __bss_start;

    volatile unsigned int *begin = (volatile unsigned int *)&__bss_start;
    volatile unsigned int *end = (volatile unsigned int *)&_end;
    unsigned int tmp = 0;
    
    printf("clean_bss begin begin[%d] end[%d]... \r\n", begin, end);

    while (begin < end) {
        *begin = 0;
        ++begin;
    }
    
    bss_end = end;

    return;
}
