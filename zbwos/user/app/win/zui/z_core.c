
static int g_x, g_y;
static int touch_up_event;  //触屏抬起消息

/* 上层获取触屏事件（目前只有抬起） */
int zui_touch_event_poll(int *xy) {
    if (touch_up_event) {
        touch_up_event = 0;
        *xy = (g_x << 16) + g_y;
        return 0;
    }
    return -1;
}

/* 底层触屏事件回调 */
int zui_touch_event_loop(int x, int y, int press) {
    static int press_bak = 0;
    
    g_x = x;
    g_y = y;

    if (!press && press_bak) {  //触屏已抬起并且上次是按下，触发抬起消息
        touch_up_event = 1;
        printf("zui_touch_event x[%d] y[%d] press[%d]\r\n", x, y, press);
    }
    
    press_bak = press;
    
    return 0;
}

/* zui初始化 */
int zui_init() {
    touchevent_register(zui_touch_event_loop);
    return 0;
}
