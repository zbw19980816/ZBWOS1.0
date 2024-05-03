
#include "lcd.h"

/* 实现画点 */

/* 获得LCD参数 */
static unsigned int fb_base;
static int xres, yres, bpp;

void fb_get_lcd_params(void)
{
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* rgb: 0x00RRGGBB */
unsigned short convert32bppto16bpp(unsigned int rgb)
{
    int r = (rgb >> 16)& 0xff;
    int g = (rgb >> 8) & 0xff;
    int b = rgb & 0xff;

    /* rgb565 */
    r = r >> 3;
    g = g >> 2;
    b = b >> 3;

    return ((r<<11) | (g<<5) | (b));
}


/* color : 32bit, 0x00RRGGBB
 *
 */
void fb_put_pixel(int x, int y, unsigned int color)
{
    unsigned char  *pc;  /* 8bpp */
    unsigned short *pw;  /* 16bpp */
    unsigned int   *pdw; /* 32bpp */

    unsigned int pixel_base = fb_base + (xres * bpp / 8) * y + x * bpp / 8;
    //printf("bpp %d\r\n", bpp);  //8
    switch (bpp)
    {
        case 8:
            pc = (unsigned char *) pixel_base;
            *pc = color;
            break;
        case 16:
            pw = (unsigned short *) pixel_base;
            *pw = convert32bppto16bpp(color);
            break;
        case 32:
            pdw = (unsigned int *) pixel_base;
            *pdw = color;
            break;
    }
}

/* 绘制实心矩形 */
void fb_put_pixel_2(    int x, //中心点x
                        int y, //中心点y
                        unsigned int color, //色彩
                        int z  //中心点到边缘距离
) {
    int i, j;

    for (i = x; i < x + z; ++i)
        for (j = y; j < y + z; ++j)
    fb_put_pixel(i, j, 0xff00);

    return;
}

#include "stl.h"
static unsigned int fb_base_dou = NULL;

/* color : 32bit, 0x00RRGGBB
 *
 */
void fb_put_pixel_dou(int x, int y, unsigned int color)
{
    unsigned char  *pc;  /* 8bpp */
    unsigned short *pw;  /* 16bpp */
    unsigned int   *pdw; /* 32bpp */

    if (NULL == fb_base_dou) {
        fb_base_dou = New(480*272*2);
        printf("dou_refresh fb_base_dou[%d]\r\n",fb_base_dou);
        }

    unsigned int pixel_base = fb_base_dou + (xres * bpp / 8) * y + x * bpp / 8;
    //printf("bpp %d\r\n", bpp);  //8
    switch (bpp)
    {
        case 8:
            pc = (unsigned char *) pixel_base;
            *pc = color;
            break;
        case 16:
            pw = (unsigned short *) pixel_base;
            *pw = convert32bppto16bpp(color);
            break;
        case 32:
            pdw = (unsigned int *) pixel_base;
            *pdw = color;
            break;
    }
}

void dou_refresh() {
    if (NULL == fb_base_dou)
            fb_base_dou = New(480*272*2);
    printf("dou_refresh fb_base_dou[%d]",fb_base_dou);

    unsigned int *p = fb_base;
    memcpy(p, fb_base_dou, (480*272*2));

}

void pri_framebuf()
{
    unsigned int *p = fb_base;
    int i = 0;

    for (i = 0; i < 480*272*2 / 4; ++i) {
        printf("[%d]",p[i]);
    }
}

void dou_clear() {
    unsigned int *p = fb_base;
    memset(p, 0, (480*272*2 / 2));

}

