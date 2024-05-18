#ifndef __TOUCH_PROC_H_
#define __TOUCH_PROC_H_

typedef int(* touch_func)(int, int, int);

int touchevent_register(touch_func fp);
int touchevent_unregister(int id);
void touch_task_init();

#endif

