#ifndef __TASK_H_
#define __TASK_H_

#define PRIO_MAX 32         /* 支持优先级数量 */
#define SYSTIMECYL 10       /* 系统时钟周期 */
#define TICKLISTSIZE 10     /* 延时链表数量，每个链表对应一种时间，用于优化查找效率 */
#define STACK_SIZE 4096     /* 默认task栈大小 */

/* 任务状态 */
typedef enum{
    TASK_SPD = 0u,  //挂起
    TASK_RDY,       //就绪
    TASK_TICK,      //延时
    TASK_SEM,       //等待信号量产生延时
} TASKSTATE;


/* 任务控制块 */
typedef struct task_ctrl TASK_CTRL;
struct task_ctrl{
    /* 
    |:未使用     =:已使用
    低地址：stack_base                          stack     高地址
                [||||||||||||||||||||||||||||||==========]
    */
    int *stack;                 /* 栈顶地址 */
    int *stack_base;            /* 栈下限地址 */
    
    TASKSTATE TaskState;        /* 任务状态 */
    
    unsigned int dlySysTime;    /* 延时到时的系统时间 */

    TASK_CTRL *PrevPtr;         /* 上个节点 */
    TASK_CTRL *NextPtr;         /* 下个节点 */

    unsigned char prio;         /* 优先级 */
    char res[3];
};

/* 就绪链表 */
typedef struct{
    TASK_CTRL *HeadPoint;
    TASK_CTRL *TailPoint;
    unsigned int TaskNum; /* 记录当前优先级任务数量 */
} RDY_LIST;

typedef void (* task_function)(TASK_CTRL *task_ctrl, void *param);

TASK_CTRL* creat_task(task_function f, void *param, unsigned char prio, int stack_size);
void start_task();
void task_exit(TASK_CTRL *task_ctrl);
void msleep(unsigned int time);


#endif
