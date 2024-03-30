#include "shell.h"
#include "stl.h"
#include "corectrl.h"
#include "task.h"
#include "my_printf.h"
#include "timer.h"
#include "s3c2440_soc.h"

char g_opendebug = 0;  /* 是否打开debug */

/* 设置调试状态 */
void setdebug(char set) {
    g_opendebug = set;
    return;
}

/* 获取调试状态 */
char getdebug(void) {
    return g_opendebug;
}
extern int Criticalcnt;

/* 调试 */
void debug(const char *fmt, ...)
{
    if (g_opendebug == 0) return;

    Enter_Critical();  ///退不出了？？？？
    //TCON &= ~(1<<0);

    va_list ap;

    va_start(ap, fmt);
    my_vprintf(fmt, ap);	
    va_end(ap);

    Exit_Critical();
    //TCON |= (1<<0);
   // pri_byte("g_opendebug == ",  Criticalcnt);
    return;
}

/* 内核初始化 */
void core_init() {
    //创建shell
    
    puts("core_init 111 \r\n");
    setdebug(0);
    puts("core_init 222 \r\n");
    creat_task(task_shell, NULL, PRIO_MAX - 1, STACK_SIZE);
    puts("core_init 333 \r\n");
    return;
}

