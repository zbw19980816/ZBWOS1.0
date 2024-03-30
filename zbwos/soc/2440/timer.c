
#include "s3c2440_soc.h"

int cur_task = -1;   //当前任务

void timer_irq(int lr)
{
puts("do_irq_2440\r\n");
#if 0
    /* 点灯计数 */
    static int cnt = 0;
    int tmp;

    cnt++;

    tmp = ~cnt;
    tmp &= 7;
    GPFDAT &= ~(7<<4);
    GPFDAT |= (tmp<<4);
#endif

}

/* clk为调度周期（ms），可选1~1000 */
//TCNTO0: 从TCNTB0开始自减，减到0时触发中断
int timer_init(unsigned int clk)
{
    /* 设置TIMER0的时钟 */
    /* Timer clk = PCLK / {prescaler value+1} / {divider value} 
                 = 50000000/(49+1)/16
                 = 62500
     */
    TCFG0 = 49;  /* Prescaler 0 = 99, 用于timer0,1 */
    TCFG1 &= ~0xf;
    TCFG1 |= 3;  /* MUX0 : 1/16 */

    /* 设置TIMER0的初值,若TCNTB0=62500，则1s中断一次 */
    if (clk <= 1000 && clk >= 1) {
        TCNTB0 = 62500 * clk / 1000;
    } else {
        puts("illegal clk");
        return -1;
    }
    
    /* 加载初值, 启动timer0 */
    TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */

    /* 设置为自动加载并启动 */
    TCON &= ~(1<<1);
    TCON |= (1<<0) | (1<<3);  /* bit0: start, bit3: auto reload */
    //TCON &= ~(1<<0);
    //debug("!!!!!!!!!!!!!!!!!!!! TCNTO0 = %d\r\n", TCNTO0);
    //TCON |= (1<<0);

    /* 设置中断 */
    register_irq(10, timer_irq);

    /*
        
        debug("111 TCNTO0 = %d\r\n", TCNTO0);
        debug("111 aa TCNTO0 = %d\r\n", TCNTO0);
        dalay(1000);
        debug("222 TCNTO0 = %d\r\n", TCNTO0);
        
        TCON &= ~(1<<0);  //关
        dalay(1000);
        debug("3 TCNTO0 = %d\r\n", TCNTO0);
        dalay(1000);
        debug("333 TCNTO0 = %d\r\n", TCNTO0);

        
        TCON |= (1<<0);  //开
            
        debug("4 TCNTO0 = %d\r\n", TCNTO0);
        dalay(1000);
        debug("5 TCNTO0 = %d\r\n", TCNTO0);

    TCNTO0 = 1000;
    debug("4 TCNTO0 = %d\r\n", TCNTO0);
            dalay(1000);
            debug("5 TCNTO0 = %d\r\n", TCNTO0);

        
    while (1) {}*/
    return ;
}

 int Criticalcnt = 0;

/* 退出临界区 */
void Exit_Critical() {
    if (--Criticalcnt == 0) {
        TCON |= (1<<0);
       // TCNTB0 = 10;  //快速触发中断，nop2
    } return;
}

/* 进入临界区 */
void Enter_Critical() {
    TCON &= ~(1<<0);
    Criticalcnt++;
    return;
}

