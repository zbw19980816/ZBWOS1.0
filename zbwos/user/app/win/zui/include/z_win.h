
#ifndef Z_WIN_H
#define Z_WIN_H


#include "jpeg_proc.h"


//控件类型
typedef enum {
    Z_STATIC = 0,
} WIN_CONTROL_TYPE_E;

//消息类型
typedef enum {
    Z_EVENT_INIT = 0,   //初始化
    Z_EVENT_TOUCH,      //触屏事件
    Z_EVENT_DESTROY,    //销毁
    Z_EVENT_CONTROL,    //内部控件消息
} WIN_EVENT_E;

//控件消息类型
typedef enum {
    Z_CONTROL_TOUCH_DOWN = 0,   //触屏按下
} WIN_CONTROL_EVENT_E;

//控件结构
typedef struct {
    int x;
    int y;
    int w;
    int h;
    int id;
    WIN_CONTROL_TYPE_E type;
    BITMAP_S *bitmap;
    char *text;
} WIN_CONTROL_T;

//窗口句柄
struct win_hwd {
    int id;
    int control_num;            //control num
    WIN_CONTROL_T *control;     //control arr
    struct win_hwd* parent;     //parent
    int queue_id;
};
typedef struct win_hwd WIN_HWD;

//内部消息
typedef struct {
    int has_msg;        //是否有消息
    WIN_HWD* hwd;
    WIN_EVENT_E msg;
    void* event;
} WIN_MSG_T;

//窗口事件回调
typedef int(* Z_WIN_PROC_FUN)(  WIN_HWD*,       //窗口句柄
                                WIN_EVENT_E,    //事件类型
                                void*           //事件
);

int z_creat_model_win(      WIN_HWD *parent_hwd,        //parent
                            WIN_CONTROL_T *control,     //control arr
                            int control_num,            //control num
                            Z_WIN_PROC_FUN win_proc     //msgproc
                            );
#endif

