
#ifndef SDRAM_H
#define SDRAM_H

/* 初始化sdram */
void sdram_init();

/* sdram读写测试 */
int sdram_test(volatile unsigned char *p, unsigned int n);


#endif


