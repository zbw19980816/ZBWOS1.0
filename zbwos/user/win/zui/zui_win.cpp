#include "zui_win.hpp"

extern "C" {
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
}


ZUI_WIN::ZUI_WIN() {
    printf("ZUI_WIN::ZUI_WIN\r\n");
    //this->rgbNum = 0;
    
}

ZUI_WIN::~ZUI_WIN() {
    printf("ZUI_WIN::~ZUI_WIN\r\n");
}

//添加一个控件
/*int ZUI_WIN::zui_add_control(ZUI_CONTROL &control) {
    printf("ZUI_WIN::zui_add_control\r\n");
   // control_vector.push_back(control);
    return 0;
}*/

#if 0
//增加一个rgb缓存
int ZUI_WIN::add_rgbBuf(char* rgbBuf) {
    if (this->rgbNum >= ZUI_LOAD_RGB_MAX) {
        return -1;
    }

   // this->rgbBuf[rgbNum] = rgbBuf;
   // this->rgbNum++;
    return 0;
}


int ZUI_WIN::zui_win_show()
{
    debug("zui_win_show\r\n");
    return 0;
}

int zui_init() {

    return 0;
}

int add(int a, int b) {
    return a+b;
}

#endif
