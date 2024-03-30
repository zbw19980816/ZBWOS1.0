#include "zui_win_test.hpp"


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
    #include "jpeg_proc.h"
    
}

//主窗口测试
ZUI_WIN_TEST::ZUI_WIN_TEST() {
    printf("ZUI_WIN_TEST::ZUI_WIN_TEST\r\n");

    //1、添加控件
    /*ZUI_CONTROL_STATIC control;
    control.x = 10;
    control.y = 10;
    control.w = 200;
    control.y = 200;
    zui_add_control(control);*/
    //2、绑定控件和消息处理方法

    
    //加载jpeg
    //jpeg_decode_from_file("huaji.jpeg", &this->rgbBuf[0]);
    //this->rgbNum = 1;
    
}

ZUI_WIN_TEST :: ~ZUI_WIN_TEST() {
    printf("ZUI_WIN_TEST::~ZUI_WIN_TEST\r\n");
}


/*
int ZUI_WIN_TEST::zui_win_test_show()
{
    debug("zui_win_show\r\n");
    return 0;
}
*/
