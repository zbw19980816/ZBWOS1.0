
#include "geometry.h"
#include "font.h"

void lcd_test(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;
	int x, y;
	unsigned char *p0;
	unsigned short *p;
	unsigned int *p2;
    puts("lcd_init  \r\n");
		
	/* 初始化LCD */
	lcd_init();
    puts("lcd_enable  \r\n");

	/* 使能LCD */
	lcd_enable();
    puts("get_lcd_params  \r\n");

	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();
	
    puts("bpp  \r\n");
	/* 往framebuffer中写数据 */
	if (bpp == 8)
	{
		/* 让LCD输出整屏的红色 */

		/* bpp: palette[12] */

		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 12;

		/* palette[47] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 47;

		/* palette[88] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 88;

		/* palette[0] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 0;
			
	}
	else if (bpp == 16)
	{
		/* 让LCD输出整屏的红色 */

		/* 565: 0xf800 */

		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0xf800;

		/* green */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x7e0;

		/* blue */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x1f;

		/* black */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0;
			
	}
	else if (bpp == 32)
	{
		/* 让LCD输出整屏的红色 */

		/* 0xRRGGBB */

		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0xff0000;

		/* green */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x00ff00;

		/* blue */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x0000ff;

		/* black */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0;

	}
    puts("bpp 1111<<<<<< \r\n");

	//msleep(3000);
	#if 0
	/* 画线 */
	draw_line(0, 0, xres - 1, 0, 0x23ff77);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xffff);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff00aa);
	draw_line(0, 0, 0, yres - 1, 0xff00ef);
	draw_line(0, 0, xres - 1, yres - 1, 0xff45);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff0780);

	//msleep(100);

	/* 画圆 */
    puts("bpp 222<<<<<< \r\n");
	draw_circle(xres/2, yres/2, yres/4, 0xff);

	/* 输出文字 */
    puts("bpp 333<<<<<< \r\n");
	fb_print_string(100, 100, "zbwos1", 0xff0000);
	fb_print_string(120, 100, "zbwos2", 0xff00);
	fb_print_string(140, 100, "zbwos3", 0xff);
    #endif
}

