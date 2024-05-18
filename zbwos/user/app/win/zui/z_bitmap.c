
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
//#include "touch_proc.h"
#include "stl.h"
#include "z_win.h"


void z_paint_rgb(WIN_CONTROL_T *control) {
    unsigned int x = 0, y = 0, rgb, k = 0;
    char *bmp = control->bitmap->data;

    for ( y = control->y; y < control->y + control->h; ++y) {
        for ( x = control->x; x < control->x + control->w; ++x) {
            rgb = bmp[k + 2] + (bmp[k + 1] << 8) + (bmp[k] << 16);
            k += 3;
            fb_put_pixel_dou(x, y, rgb);
        }
    }
    dou_refresh();
    return;
}

