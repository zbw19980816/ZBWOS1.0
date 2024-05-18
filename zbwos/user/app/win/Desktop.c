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

/* 加载logo */
void logo_init() {
    BITMAP_S bmp;
    WIN_CONTROL_T control;
    Enter_Critical();
    jpeg_decode_from_file("Album_zbwos_logo.jpg", &bmp);
    Exit_Critical();
    control.bitmap = &bmp;
    control.x = 0;
    control.y = 0;
    control.w = 480;
    control.h = 272;
    
    Enter_Critical();
    z_paint_rgb(&control);
    Exit_Critical();

    //Delete(bmp.data);

    return;
}

/* 桌面消息处理 */
int Desktop_proc(   WIN_HWD* hwd, WIN_EVENT_E msg, void* event) {
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
            printf("Z_EVENT_CONTROL, id[%d], code[%d]\r\n", id, code);
            switch (id) {
                case SETTING_CONTROL_ID:
                    //进入设置界面
                    printf("Settint_win_creat>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
                    Settint_win_creat(hwd);
                    printf("Settint_win_creat<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\r\n");
                    z_win_notify_msg(hwd, Z_EVENT_INIT, NULL);   //暂时先用初始化消息重绘本窗口，后续增加刷新消息优化
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

//桌面入口
void Desktop(TASK_CTRL *task_ctrl, void *param) {
    int d = 0;
    BITMAP_S *Desktop_pic_bmp;
    BITMAP_S *Setting_pic_bmp;
    BITMAP_S *Album_pic_bmp;

    //获取桌面资源
    pic_res_proc(RES_PROC_TYPE_GET_DESKTOP_BAK, &Desktop_pic_bmp);
    pic_res_proc(RES_PROC_TYPE_GET_SETTING_BTN, &Setting_pic_bmp);
    pic_res_proc(RES_PROC_TYPE_GET_ALBUM_BTN, &Album_pic_bmp);

    WIN_CONTROL_T control[] = {   //WIN_HWD hwd;
        {0, 0, 480, 272, DESKTOP_CONTROL_ID, Z_STATIC, Desktop_pic_bmp, NULL},      //壁纸
        {16, 16, 32, 32, SETTING_CONTROL_ID, Z_STATIC, Setting_pic_bmp, NULL},      //设置
        {16, 64, 32, 32, ALBUM_CONTROL_ID,   Z_STATIC, Album_pic_bmp, NULL},        //图库
    };

    //logo
    logo_init();
    
    //初始化资源
    pic_res_proc(RES_PROC_TYPE_INIT, NULL);

    //创建桌面
    printf("Desktop_proc >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    z_creat_model_win(NULL, control, sizeof(control) / sizeof(control[0]), Desktop_proc);
    printf("Desktop_proc <<<<<<<<<<<<<\r\n");

    return;
}

/* 图片资源操作 */
int pic_res_proc(RES_PROC_TYPE type, BITMAP_S** bmp) {
    int i;
    static RES_TABLE s_res[] = {
        {RES_PROC_TYPE_GET_DESKTOP_BAK, "Album_Desktop.jpg"},
        {RES_PROC_TYPE_GET_SETTING_BTN, "Album_setting_btn.jpg"},
        {RES_PROC_TYPE_GET_ALBUM_BTN,   "Album_album_btn.jpg"},
        {RES_PROC_TYPE_GET_SETTING_BAK, "Album_setting_bak.jpg"},
        {RES_PROC_TYPE_GET_DESTROY_BTN, "Album_destroy_btn.jpg"},
    };

    if (type > RES_PROC_TYPE_INIT)
        return -1;
    
    if (RES_PROC_TYPE_INIT == type) {       //初始化所有资源
        Enter_Critical();
        for (i = 0; i < sizeof(s_res) / sizeof(s_res[0]); ++i)
            jpeg_decode_from_file(s_res[i].picname, &s_res[i].bmp);
        Exit_Critical();
        return 0;
    }

    //查找资源
    for (i = 0; i < sizeof(s_res) / sizeof(s_res[0]); ++i)
        if (type == s_res[i].type) {
            *bmp = &s_res[i].bmp;
            return 0;
        }
        
    return -1;
}

/* 桌面线程初始化 */
void Desktop_task_init() {
    //初始化zui
    zui_init();

    //创建桌面
    creat_task(Desktop, NULL, 0, STACK_SIZE);
    
    return;
}

