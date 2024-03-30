
#ifndef __ZUI_CONTROL_STATIC_H_
#define __ZUI_CONTROL_STATIC_H_

#include "zui_control.hpp"

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
    #include "stl.h"
}


#if 1
template<class C>  //template后面跟的类就是类模板
class ZUI_VECTOR 
{ 
public :
    class LIST {
    public:
        C *cls;
        LIST *next;
    } cls_list;      //模板链表
    
    ZUI_VECTOR() {
        cls_list.cls = (C *)NULL;
        cls_list.next = (LIST *)NULL;
    }

    //插入
    void push_back(C &cls) {
        //新增对象
        C *newcls = (C *)New(sizeof(C));
        *newcls = cls;
        
        //新增节点
        LIST *new_cls_list = (LIST *)New(sizeof(new_cls_list));
        new_cls_list->cls = newcls;
        new_cls_list->next = (LIST *)NULL;

        //节点插入
        LIST *p_cls_list = &cls_list;
        while (p_cls_list->next)
            p_cls_list = p_cls_list->next;
        p_cls_list->next = new_cls_list;

        return;
    }
/*
    ~ZUI_VECTOR() {
        LIST *p_cls_list = &cls_list;
        C *cls;
        while (p_cls_list->next) {
            Delete(p_cls_list->next->cls);
            Delete(p_cls_list->next);
            p_cls_list = p_cls_list->next;
        }
    }
*/
};

#endif

#endif

