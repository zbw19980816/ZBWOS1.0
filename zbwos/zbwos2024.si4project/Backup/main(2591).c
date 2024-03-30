
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

    debug("task_d ... task_ctrl = %d\r\n", task_ctrl);
    Enter_Critical();
    jpeg_decode_from_file("huaji.jpg", &buf);
    lcd_test();
    Exit_Critical();

    
    showRGB(buf);

    while (1) {
        debug("11task_d ...222 task_ctrl = %d\r\n", task_ctrl);
        msleep(2500);
    }
}
#endif
int main() {

    nand_init();
    filesystem_init();
    core_init();
    
    creat_task(jpeg, (void*)'d', 0, STACK_SIZE);
    //creat_task(GUI, (void*)'g', 0, STACK_SIZE);
    
    start_task();

    return 0;
}

