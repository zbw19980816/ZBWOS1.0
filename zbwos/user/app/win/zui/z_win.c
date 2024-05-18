
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
//#include "touch_proc.h"
#include "stl.h"
#include "z_win.h"

static WIN_HWD* winhwd_vec[64] = {0};  //窗口句柄管理
static WIN_MSG_T g_msg;  //最新消息

//空闲窗口id获取
int get_winhwd_id(WIN_HWD *hwd) {
    int i = 0;

    if (!hwd)
        return -1;
    
    for (i = 0; i < sizeof(winhwd_vec) / sizeof(winhwd_vec[0]); ++i) {
        if (!winhwd_vec[i]) {
            winhwd_vec[i] = hwd;
            return i;
        }
    }

    return -1;
}

//窗口销毁，移除id占用
int clear_winhwd_id(int id) {
    int i = 0;

    if (id >= sizeof(winhwd_vec) / sizeof(winhwd_vec[0] || id < 0))
        return -1;
    
    winhwd_vec[id] = NULL;

    return 0;
}

/* 窗口默认处理 */
int win_msg_default_proc(       WIN_HWD* hwd,       //窗口句柄
                                WIN_EVENT_E msg,    //事件类型
                                void* event         //事件
) {
    int i = 0;
    int xy, x, y;
    
    //g_msg = msg;
    switch (msg) {
        case Z_EVENT_INIT:
            printf("win_msg_default_proc Z_EVENT_INIT control_num[%d] w[%d] h[%d] data[%x]  hwd->control->x[%d], hwd->control->y[%d], hwd->control->w[%d], hwd->control->h[%d]\r\n",
                hwd->control_num, hwd->control->bitmap->head.biWidth, hwd->control->bitmap->head.biHeight, hwd->control->bitmap->data[0],
                hwd->control->x, hwd->control->y, hwd->control->w, hwd->control->h);
            
            for (i = 0; i < hwd->control_num; ++i) {
                //绘制bitmap
                Enter_Critical();
                printf("for Z_EVENT_INIT control_num[%d] w[%d] h[%d] data[%x]  hwd->control->x[%d], hwd->control->y[%d], hwd->control->w[%d], hwd->control->h[%d]\r\n",
                hwd->control_num, hwd->control[i].bitmap->head.biWidth, hwd->control[i].bitmap->head.biHeight, hwd->control[i].bitmap->data[0],
                hwd->control[i].x, hwd->control[i].y, hwd->control[i].w, hwd->control[i].h);
                z_paint_rgb(&hwd->control[i]);
                Exit_Critical();
            }
            break;
        case Z_EVENT_TOUCH:
            xy = (int)event;
            x = xy >> 16;
            y = xy & 0xFFFF;
            printf("Z_EVENT_TOUCH, x[%d] y[%d]\r\n", x, y);
            
            for (i = hwd->control_num - 1; i >= 0; --i) {
                if (x >= hwd->control[i].x && x <= hwd->control[i].x + hwd->control[i].w &&
                    y >= hwd->control[i].y && y <= hwd->control[i].y + hwd->control[i].h) {
                    //顶层控件收到触屏消息
                    printf(" hwd->control.id[%d] gettttttttttt touch\r\n",  hwd->control[i].id);
                    z_win_notify_msg(hwd, Z_EVENT_CONTROL, hwd->control[i].id << 16 | Z_CONTROL_TOUCH_DOWN);
                    
                    break;
                }
            }
            
            break;
        default :
            printf("illegal msg[%d]\r\n", msg);
            break;
    }
    
    return 0;
}

/* 创建窗口 */
int z_creat_model_win(      WIN_HWD *parent_hwd,        //parent
                            WIN_CONTROL_T *control,     //control arr
                            int control_num,            //control num
                            Z_WIN_PROC_FUN win_proc     //msgproc
                            ) {
    //WIN_HWD *hwd = (WIN_HWD *)New(sizeof(WIN_HWD));
    int xy = 0;
    WIN_HWD hwd;
    WIN_MSG_T msg_t;
    
    hwd.parent = parent_hwd;
    hwd.id = get_winhwd_id(&hwd);
    hwd.control_num = control_num;
    hwd.control = control;

    win_proc(&hwd, Z_EVENT_INIT, NULL);

    //创建窗口消息队列
    hwd.queue_id = queue_creat(sizeof(WIN_MSG_T));
    printf("queue_creat  hwd->queue_id[%d] \r\n", hwd.queue_id);
    while (1) {
        msleep(50);

        //外部输入事件处理
        if (0 == zui_touch_event_poll(&xy))
            win_proc(&hwd, Z_EVENT_TOUCH, (void *)xy);

        //内部队列消息处理
        if (!z_win_msq_que_proc(&hwd, &msg_t)) {
            win_proc(msg_t.hwd, msg_t.msg, msg_t.event);
            if (Z_EVENT_DESTROY == msg_t.msg)
                break;
        }
    }
    
    queue_destroy(hwd.queue_id);
    clear_winhwd_id(hwd.id);
    
    return 0;
}

//内部消息队列
int z_win_msq_que_proc(WIN_HWD *hwd, WIN_MSG_T *msg_t) {
    int ret = 0;
    ret = queue_recv(hwd->queue_id, msg_t);
    if (!ret)
    printf("queue_recv <<< hwd->queue_id[%d], ret[%d] \r\n", hwd->queue_id, ret);
    return ret;
}

int z_win_msq_que_proc_2(WIN_MSG_T *msg_t) {
    int ret = g_msg.has_msg;
printf("ret[%d] \r\n", ret);
    if (!ret)
        return ret;
    
    memcpy(msg_t, &g_msg, sizeof(*msg_t));
    g_msg.has_msg = 0;
    
    return ret;
}

//内部消息队列投递
int z_win_notify_msg(    WIN_HWD* hwd, WIN_EVENT_E msg, void* event) {
    WIN_MSG_T win_msg;
    int ret = 0;
    
    win_msg.hwd = hwd;
    win_msg.msg = msg;
    win_msg.event = event;

    ret = queue_send(hwd->queue_id, &win_msg);
    printf("queue_send >>> hwd->queue_id[%d], ret[%d] \r\n", hwd->queue_id, ret);

    return ret;
}

int z_win_notify_msg_2(    WIN_HWD* hwd, WIN_EVENT_E msg, void* event) {
    g_msg.has_msg = 1;
    g_msg.hwd = hwd;
    g_msg.msg = msg;
    g_msg.event = event;

    return 0;
}

