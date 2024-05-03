
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


#if 1
/* 显示滑稽 480*272  140*36 */
void showbmp(char *bmp) {
    unsigned int x = 0, y = 0, rgb, k = 0;
    for ( y = 236; y > 36; --y) {
        for ( x = 140; x < 340; ++x) {
            //bmp排列为BGR    B：bmp[k]  G：bmp[k + 1]    R：bmp[k + 2]
            //fb_put_pixel入参rgb：bit0~bit7为b bit8~bit15为g bit16~bit23为r
            //自下而上，自左而右画
            rgb = bmp[k] + (bmp[k + 1] << 8) + (bmp[k + 2] << 16);
            k += 3;
            fb_put_pixel(x, y, rgb);
        }
        //msleep(50);
    }
    
}

void showRGB(char *bmp) {
    unsigned int x = 0, y = 0, rgb, k = 0;
    for ( y = 236; y > 36; --y) {
        for ( x = 140; x < 340; ++x) {
            //bmp排列为BGR    B：bmp[k]  G：bmp[k + 1]    R：bmp[k + 2]
            //fb_put_pixel入参rgb：bit0~bit7为b bit8~bit15为g bit16~bit23为r
            //自下而上，自左而右画
            rgb = bmp[k + 2] + (bmp[k + 1] << 8) + (bmp[k] << 16);
            k += 3;
            fb_put_pixel(x, y, rgb);
        }
        //msleep(50);
    }
    
}



void fb_put_pixel_dou(int x, int y, unsigned int color);
void dou_refresh();

void showRGB_dou(char *bmp) {
    unsigned int x = 0, y = 0, rgb, k = 0;
    for ( y = 236; y > 36; --y) {
        for ( x = 140; x < 340; ++x) {
            //bmp排列为BGR    B：bmp[k]  G：bmp[k + 1]    R：bmp[k + 2]
            //fb_put_pixel入参rgb：bit0~bit7为b bit8~bit15为g bit16~bit23为r
            //自下而上，自左而右画
            rgb = bmp[k + 2] + (bmp[k + 1] << 8) + (bmp[k] << 16);
            k += 3;
            fb_put_pixel_dou(x, y, rgb);
        }
        //msleep(50);
    }
    dou_refresh();
}



void GUI(TASK_CTRL *task_ctrl, void *param) {
    int i = 0;
    char *buf1;
    char *buf2;
    int ret;
    
    Enter_Critical();
    lcd_test();
    SYS_TREE * p = filesystem_open_file("huaji.bmp");
    printf("comrecv name[%s] size[%d] nandaddr[%d] \r\n", p->name, p->size, p->nandaddr);

    if (p) {
        buf1 = (char *)New(p->size);
        ret = filesystem_read_file(p, buf1, p->size);
        printf("ret = %d p->size[%d]\r\n", ret, p->size);
        //showbmp(buf1 + 54);
    }

    p = filesystem_open_file("rgb.bmp");
    printf("0127.bmp name[%s] size[%d] nandaddr[%d] \r\n", p->name, p->size, p->nandaddr);
    if (p) {
        printf("kkkk\r\n");
        buf2 = (char *)New(p->size);
        ret = filesystem_read_file(p, buf2, p->size);
        printf("ret = %d p->size[%d]\r\n", ret, p->size);
        //showbmp(buf2 + 54);
    }
    while (1) {
        printf("msleep(1000)\r\n");
        msleep(1000);
    }
    showbmp(buf1 + 54);
    msleep(3000);
    //pri_framebuf();
    
    printf("dou_clear>>>>\r\n");
    dou_clear();
    printf("dou_clear<<<\r\n");
    while (1) {
        printf("msleep(1000)\r\n");
        msleep(1000);
    }
    while (1) {
    showbmp(buf1 + 54);
   // dalay(10);
    showbmp(buf2 + 54);
   // dalay(10);
    }
    Exit_Critical();
    while (1) {
        msleep(500);
    }
}

void jpeg(TASK_CTRL *task_ctrl, void *param) {
    int d = 0;
    char *buf;
    char *buf2;

    debug("task_d ... task_ctrl = %d\r\n", task_ctrl);
    Enter_Critical();
    jpeg_decode_from_file("huaji.jpg", &buf);
    jpeg_decode_from_file("rgb.jpeg", &buf2);
    
    lcd_test();
    Exit_Critical();
    Enter_Critical();

    showRGB_dou(buf);
    Exit_Critical();
    msleep(3000);
    printf("dou_clear>>>>\r\n");
       // dou_clear();
        Enter_Critical();
        dou_refresh();
        Exit_Critical();
        printf("dou_clear<<<\r\n");
    //pri_framebuf();

    while (1) {
        
        Enter_Critical();
        printf("showRGB(buf)>>\r\n");
        showRGB_dou(buf);
       // showRGB_dou(buf);
        printf("showRGB(buf)<<\r\n");
        Exit_Critical();
        
        debug("11task_d ...222 task_ctrl = %d\r\n", task_ctrl);
        msleep(3000);
        Enter_Critical();
        printf("showRGB(buf2)>>\r\n");
        showRGB_dou(buf2);
       Exit_Critical();
       // showRGB_dou(buf2);
        printf("showRGB(buf2)<<\r\n");
        msleep(3000);
    }
}
#endif

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
    int x, y, pressure;
    
    /* 初始化LCD */
    user_lcd_init();

    //初始化touchscreen
    touchscreen_init();

    //初始化tslib
    ts_init();

    //获取触屏事件
    while (1) {
        msleep(10);
        if (ts_read(&x, &y, &pressure) == 0) {
            printf("zbw x = %d, y = %d\n\r", x, y);

            if (pressure) {
                Enter_Critical();
                fb_put_pixel_2(x, y, 0xff00, 4);
                Exit_Critical();
            }
        }
    }

    return;
}

int main() {

    nand_init();
    filesystem_init();
    core_init();
    
   // creat_task(jpeg, (void*)'d', 0, STACK_SIZE);
   // creat_task(GUI, (void*)'g', 0, STACK_SIZE);
    creat_task(touch, (void*)'d', 0, STACK_SIZE);
    start_task();

    return 0;
}

