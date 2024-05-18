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
#include "touch_proc.h"
#include "stl.h"
#include "z_win.h"
#include "Desktop.h"
#include "win_setting.h"


/* 设置界面消息处理 */
int Setting_win_proc(   WIN_HWD* hwd, WIN_EVENT_E msg, void* event) {
    int event_proc = 0;
    int id, code;

    switch (msg) {
        case Z_EVENT_INIT:
            break;
        case Z_EVENT_TOUCH:
            break;
        case Z_EVENT_CONTROL:
            event_proc = (int)event;
            id = event_proc = event_proc >> 16;     //控件id
            code = event_proc & 0xFFFF;             //控件事件
            switch (id) {
                case SETTING_DESTROY_CONTROL_ID:
                    //销毁窗口
                    z_win_notify_msg(hwd, Z_EVENT_DESTROY, NULL);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    return win_msg_default_proc(hwd, msg, event);
}

/* 创建设置界面 */
int Settint_win_creat(    WIN_HWD *parent_hwd) {
    int d = 0;
    BITMAP_S *bak_pic_bmp;
    BITMAP_S *destyoy_pic_bmp;
    char *buf, *buf2;

    pic_res_proc(RES_PROC_TYPE_GET_SETTING_BAK, &bak_pic_bmp);
    pic_res_proc(RES_PROC_TYPE_GET_DESTROY_BTN, &destyoy_pic_bmp);

    WIN_CONTROL_T control[] = {   //WIN_HWD hwd;
        {0, 0, 480, 272, SETTINT_BAK_CONTROL_ID, Z_STATIC, bak_pic_bmp, NULL},      //启动背景
        {480-24-10, 10, 24, 24, SETTING_DESTROY_CONTROL_ID, Z_STATIC, destyoy_pic_bmp, NULL},      //销毁按钮
    };

    //创建桌面
    z_creat_model_win(parent_hwd, control, sizeof(control) / sizeof(control[0]), Setting_win_proc);

    return 0;
}
