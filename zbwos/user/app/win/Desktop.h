#ifndef __DESKTOP_H_
#define __DESKTOP_H_



//控件ID
#define DESKTOP_CONTROL_ID  0
#define SETTING_CONTROL_ID  1
#define ALBUM_CONTROL_ID    2

//资源操作
typedef enum {
    RES_PROC_TYPE_GET_DESKTOP_BAK,      //桌面背景
    RES_PROC_TYPE_GET_SETTING_BTN,      //设置按钮
    RES_PROC_TYPE_GET_ALBUM_BTN,        //图库按钮
    RES_PROC_TYPE_GET_SETTING_BAK,      //设置背景
    RES_PROC_TYPE_GET_DESTROY_BTN,      //关闭窗口按钮

    //在此项之前添加
    RES_PROC_TYPE_INIT,                 //资源初始化(一定是最后一项)
} RES_PROC_TYPE;

//资源汇总表
typedef struct {
    RES_PROC_TYPE type;
    char* picname;
    BITMAP_S bmp;
} RES_TABLE;

#endif

