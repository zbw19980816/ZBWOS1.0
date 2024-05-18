#include "nandflash.h"
#include "filesystem.h"
#include "task.h"
#include "corectrl.h"
#include "task.h"
#include "framebuffer.h"
#include "timer.h"
#include "lcd.h"
#include "my_printf.h"
#include "heap.h"
#include "stl.h"
#include "touch_proc.h"

/* 触屏事件订阅池 */
static touch_func func_pool[32] = {NULL};

/* 初始化LCD */
void user_lcd_init() {
    unsigned int fb_base;
    int xres, yres, bpp;

    Enter_Critical();

    /* 初始化LCD */
    lcd_init();
    puts("lcd_enable  \r\n");

    /* 使能LCD */
    lcd_enable();
    puts("get_lcd_params  \r\n");

    /* 获得LCD的参数 */
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
    fb_get_lcd_params();
    font_init();

    /* 往framebuffer中写数据(全黑) */
    memset(fb_base, 0, xres * yres * bpp / 8);
    
    Exit_Critical();

    return;
}

/* 触屏线程 */
void touch(TASK_CTRL *task_ctrl, void *param) {
    int i;
    int x, y, pressure;
    
    /* 初始化LCD */
    user_lcd_init();

    //初始化touchscreen
    touchscreen_init();

    //初始化tslib
    ts_init();

    //event loop
    while (1) {
        msleep(100);
        if (!ts_read(&x, &y, &pressure)) {
            //printf("---x[%d]y[%d]pressure[%d]\r\n", x, y, pressure);
            for (i = 0; i < sizeof(func_pool) / sizeof(func_pool[0]); ++i)
                if (func_pool[i])
                    func_pool[i](x, y, pressure); //发布触屏事件
        }
    }

    return;
}

/* 触屏线程初始化 */
void touch_task_init() {
    creat_task(touch, NULL, 0, STACK_SIZE);
    return;
}

/* 订阅触屏事件 */
int touchevent_register(touch_func fp) {
    int i = 0;

    if (!fp)
        return -1;
    
    for (i = 0; i < sizeof(func_pool) / sizeof(func_pool[0]); ++i) {
        if (!func_pool[i]) {
            func_pool[i] = fp;
            return i;
        }
    }
    
    return -1;
}

/* 注销触屏事件 */
int touchevent_unregister(int id) {
    if (id < 0 || id >= sizeof(func_pool) / sizeof(func_pool[0]) || !func_pool[id])
        return -1;
    
    func_pool[id] = NULL;
    
    return 0;
}

