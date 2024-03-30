
#ifndef UART_H
#define UART_H

/* 初始化UART0 */
void Uart_Init();


/* 发送一个byte */
void putchar(volatile unsigned char c);


/* 发送多个byte */
void puts(unsigned char *s);


/* 接收一个byte */
volatile unsigned char getchar(void);


/* 打印变量32位变量a的值 */
void pri(char* s, int a);

#endif


