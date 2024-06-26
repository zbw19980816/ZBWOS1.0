
#include "lcd.h"
#include "lcd_controller.h"
#include "../s3c2440_soc.h"

#define HCLK 100

void jz2440_lcd_pin_init(void)
{
    puts("jz2440_lcd_pin_init 1111 \r\n");

    /* 初始化引脚 : 背光引脚 */
    GPBCON &= ~0x3;
    puts("jz2440_lcd_pin_init 222 \r\n");
    GPBCON |= 0x01;
    puts("jz2440_lcd_pin_init 333 \r\n");

    /* LCD专用引脚 */
    //GPCCON = 0xaaaaaaaa;  //zbw:影响启动
    puts("jz2440_lcd_pin_init 444 \r\n");
    GPDCON = 0xaaaaaaaa;
    puts("jz2440_lcd_pin_init 555 \r\n");

    /* PWREN */
    GPGCON |= (3<<8);
    puts("jz2440_lcd_pin_init 666 \r\n");
    return;
}


/* 根据传入的LCD参数设置LCD控制器 */
void s3c2440_lcd_controller_init(p_lcd_params plcdparams)
{
    int pixelplace;
    unsigned int addr;
puts("jz2440_lcd_pin_init \r\n");
    jz2440_lcd_pin_init();
    
    puts("jz2440_lcd_pin_init <<<<\r\n");
    /* [17:8]: clkval, vclk = HCLK / [(CLKVAL+1) x 2]
     *                   9   = 100M /[(CLKVAL+1) x 2], clkval = 4.5 = 5
     *                 CLKVAL = 100/vclk/2-1
     * [6:5]: 0b11, tft lcd
     * [4:1]: bpp mode
     * [0]  : LCD video output and the logic enable/disable
     */
    int clkval = (float)HCLK/plcdparams->time_seq.vclk/2-1+0.5;
    puts("float 111\r\n");
    //int clkval = 5;
    int bppmode = plcdparams->bpp == 8  ? 0xb :\
                  plcdparams->bpp == 16 ? 0xc :\
                  0xd;  /* 0xd: 24,32bpp */
    
    puts("LCDCON1 111\r\n");
    LCDCON1 = (clkval<<8) | (3<<5) | (bppmode<<1) ;
    puts("s3c2440_lcd_controller_init 1111  \r\n");

    /* [31:24] : VBPD    = tvb - 1
     * [23:14] : LINEVAL = line - 1
     * [13:6]  : VFPD    = tvf - 1
     * [5:0]   : VSPW    = tvp - 1
     */
    LCDCON2 =     ((plcdparams->time_seq.tvb - 1)<<24) | \
                ((plcdparams->yres - 1)<<14)         | \
                ((plcdparams->time_seq.tvf - 1)<<6)  | \
                ((plcdparams->time_seq.tvp - 1)<<0);

    /* [25:19] : HBPD     = thb - 1
     * [18:8]  : HOZVAL  = 列 - 1
     * [7:0]   : HFPD     = thf - 1
     */
    LCDCON3 =    ((plcdparams->time_seq.thb - 1)<<19) | \
                ((plcdparams->xres - 1)<<8)              | \
                ((plcdparams->time_seq.thf - 1)<<0);

    /* 
     * [7:0]   : HSPW     = thp - 1
     */
    puts("s3c2440_lcd_controller_init 2222222  \r\n");
    LCDCON4 =    ((plcdparams->time_seq.thp - 1)<<0);

    /* 用来设置引脚极性, 设置16bpp, 设置内存中象素存放的格式
     * [12] : BPP24BL
     * [11] : FRM565, 1-565
     * [10] : INVVCLK, 0 = The video data is fetched at VCLK falling edge
     * [9]  : HSYNC是否反转
     * [8]  : VSYNC是否反转
     * [7]  : INVVD, rgb是否反转
     * [6]  : INVVDEN
     * [5]  : INVPWREN
     * [4]  : INVLEND
     * [3]  : PWREN, LCD_PWREN output signal enable/disable
     * [2]  : ENLEND
     * [1]  : BSWP
     * [0]  : HWSWP
     */

    pixelplace = plcdparams->bpp == 32 ? (0) : \
                 plcdparams->bpp == 16 ? (1) : \
                 (1<<1);  /* 8bpp */
    
    LCDCON5 = (plcdparams->pins_pol.vclk<<10) |\
              (plcdparams->pins_pol.rgb<<7)   |\
              (plcdparams->pins_pol.hsync<<9) |\
              (plcdparams->pins_pol.vsync<<8) |\
               (plcdparams->pins_pol.de<<6)    |\
              (plcdparams->pins_pol.pwren<<5) |\
              (1<<11) | pixelplace;

    /* framebuffer地址 */
    /*
     * [29:21] : LCDBANK, A[30:22] of fb
     * [20:0]  : LCDBASEU, A[21:1] of fb
     */
    puts("s3c2440_lcd_controller_init 3333333  \r\n");
    addr = plcdparams->fb_base & ~(1<<31);
    LCDSADDR1 = (addr >> 1);

    /* 
     * [20:0] : LCDBASEL, A[21:1] of end addr
     */
    addr = plcdparams->fb_base + plcdparams->xres*plcdparams->yres*plcdparams->bpp/8;
    addr >>=1;
    addr &= 0x1fffff;
    LCDSADDR2 = addr;//    
    puts("s3c2440_lcd_controller_init 4  \r\n");
    return;
}

void s3c2440_lcd_controller_enalbe(void)
{
    /* 背光引脚 : GPB0 */
    GPBDAT |= (1<<0);
    
    /* pwren    : 给LCD提供AVDD  */
    LCDCON5 |= (1<<3);
    
    /* LCDCON1'BIT 0 : 设置LCD控制器是否输出信号 */
    LCDCON1 |= (1<<0);
}

void s3c2440_lcd_controller_disable(void)
{
    /* 背光引脚 : GPB0 */
    GPBDAT &= ~(1<<0);

    /* pwren    : 给LCD提供AVDD  */
    LCDCON5 &= ~(1<<3);

    /* LCDCON1'BIT 0 : 设置LCD控制器是否输出信号 */
    LCDCON1 &= ~(1<<0);
}


/* 设置调色板之前, 先关闭lcd_controller */
void s3c2440_lcd_controller_init_palette(void)
{
    volatile unsigned int *palette_base =  (volatile unsigned int *)0x4D000400;
    int i;

    int bit = LCDCON1 & (1<<0);

    /* LCDCON1'BIT 0 : 设置LCD控制器是否输出信号 */
    if (bit)
        LCDCON1 &= ~(1<<0);

    for (i = 0; i < 256; i++)
    {
        /* 低16位 : rgb565 */    
        *palette_base++ = i;
    }

    if (bit)
        LCDCON1 |= (1<<0);
}

struct lcd_controller s3c2440_lcd_controller = {
    .name    = "s3c2440",
    .init    = s3c2440_lcd_controller_init,
    .enable  = s3c2440_lcd_controller_enalbe,
    .disable = s3c2440_lcd_controller_disable,
    .init_palette = s3c2440_lcd_controller_init_palette,
};


void s3c2440_lcd_contoller_add(void)
{
    register_lcd_controller(&s3c2440_lcd_controller);
}

