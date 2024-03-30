
#ifndef __ZUI_H_
#define __ZUI_H_

#include "zui_control.hpp"
//#include "zui_vector.hpp"


//#define ZUI_LOAD_RGB_MAX    16    //单个窗口最大RGB缓存数量

/* 窗口基类 */
class ZUI_WIN {
public:
    ZUI_WIN();
    ~ZUI_WIN();
   // int zui_add_control(ZUI_CONTROL &control);//添加一个控件
    //int zui_win_show();
    //int add_rgbBuf(char* rgbBuf);
    
//private:
    //unsigned int rgbNum = 0;                    //RGB缓存数量
    //char *rgbBuf[ZUI_LOAD_RGB_MAX] = {0};       //RGB缓存
    //ZUI_VECTOR<ZUI_CONTROL> control_vector;      //控件容器
};

#endif

