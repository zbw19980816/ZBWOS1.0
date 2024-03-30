/* 流水灯 */
#include "s3c2440_soc.h"

void dalay(int d) {
    while (d--);
    return;
}

void led_init(void)
{
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
}


int led_test()
{
    unsigned int *pGPFDAT = (unsigned int *)0x56000054;
    int count = 0;
    
    //将GPF4、GPF5和GPF6设置为输出模式（gpf4接入led1，gpf5接入led2，gpf6接入led3）
    //led_init();
    
    while (1) {
        *pGPFDAT &= ~(7 << 4);   //清零，全部点亮
        *pGPFDAT |= (count << 4);  //置1，熄灭部分
        count++;
        if (count == 8)count = 0;
        dalay(100000);
    }
    
    return 0;
}


