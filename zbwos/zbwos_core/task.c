#include "task.h"
#include "stl.h"
#include "s3c2440_soc.h"
#include "shell.h"

int task_stacks[32];  //任务0~31栈顶位置记录
int task_count;


unsigned int gPrioTbl[(PRIO_MAX - 1) / sizeof(unsigned int) + 1]; /* 优先级位映射表 */
RDY_LIST gRdyList[PRIO_MAX]; /* 就绪链表 */
TASK_CTRL* gpCurTaskCtrl = NULL;    /* 当前运行任务的控制块 */
unsigned int SysTickTime = 0;  /* 系统运行时间：SysTickTime * TASK_TICK (ms) */
TASK_CTRL *TickList[TICKLISTSIZE] = {0};  /* 延时链表（已排序的单向链表） */
static char sleepwait = 0;

/* sleep后快速触发中断到来 */
void Sleepwait() {
    sleepwait = 1;
    //TCNTB0 = 2;  //快速触发中断，nop2
    while(sleepwait) {
       // puts("a\r\n");
       // dalay(1000);
    } return;
}

/* 优先级位映射表置位 */
static void PrioSet(unsigned char ucPrio) {
    if ((ucPrio / 32) >= (sizeof(gPrioTbl) / sizeof(gPrioTbl[0]))) {
        printf("PrioSet fail, ucPrio[%d]", (int)ucPrio);
        return;
    }
    
    gPrioTbl[ucPrio / 32] |= (0x01 << (0x1F - (ucPrio & 0x1F)));
    return;
}

/* 就绪列表尾部插入节点 */
static void RdyList_TailInsert(TASK_CTRL *task_ctrl) {
    task_ctrl->NextPtr = NULL;
    task_ctrl->PrevPtr = gRdyList[task_ctrl->prio].TailPoint;
    if (gRdyList[task_ctrl->prio].TaskNum == 0) {
        gRdyList[task_ctrl->prio].HeadPoint = task_ctrl;
        gRdyList[task_ctrl->prio].TailPoint = task_ctrl;
    } else {
        gRdyList[task_ctrl->prio].TailPoint->NextPtr = task_ctrl;
        gRdyList[task_ctrl->prio].TailPoint = task_ctrl;
    }
    
    task_ctrl->TaskState = TASK_RDY;
    gRdyList[task_ctrl->prio].TaskNum++;
    
    /* 置位相应优先级 */
    PrioSet(task_ctrl->prio);

    return;
}

/* 优先级位映射表清除 */
static void PrioRemove(unsigned char ucPrio) {
    gPrioTbl[ucPrio / 32] &= (~(0x01 << (0x1F - (ucPrio & 0x1F))));
    return;
}

/* 就绪列表移除节点 */
static void RdyListRemove(TASK_CTRL* pCurTaskCtrl) {
    RDY_LIST *rdypoint = &gRdyList[pCurTaskCtrl->prio];
    if (rdypoint->TaskNum == 1) {
        rdypoint->TaskNum = 0;
        rdypoint->HeadPoint = NULL;
        rdypoint->TailPoint = NULL;
        PrioRemove(pCurTaskCtrl->prio);
    } else if(rdypoint->TaskNum >= 2) {
        if (rdypoint->HeadPoint == pCurTaskCtrl) {
            /* 头结点 */
            rdypoint->HeadPoint = rdypoint->HeadPoint->NextPtr;
            rdypoint->HeadPoint->PrevPtr = NULL;
        } else if(rdypoint->TailPoint == pCurTaskCtrl) {
            /* 尾结点 */
            rdypoint->TailPoint = rdypoint->TailPoint->PrevPtr;
            rdypoint->TailPoint->NextPtr = NULL;
        } else {
            pCurTaskCtrl->NextPtr->PrevPtr = pCurTaskCtrl->PrevPtr;
            pCurTaskCtrl->PrevPtr->NextPtr = pCurTaskCtrl->NextPtr;
        }
        
        rdypoint->TaskNum--;
        pCurTaskCtrl->NextPtr = NULL;
        pCurTaskCtrl->PrevPtr = NULL;
    } return;
}

/* 创建task */
TASK_CTRL* creat_task(task_function f, void *param, unsigned char prio, int stack_size) {
    TASK_CTRL *task_ctrl = NULL;
    int *top = NULL;
    puts("creat_task 111 \r\n");

    /* 1、初始化控制块和栈空间 */
    task_ctrl = New(sizeof(TASK_CTRL));
    if (!task_ctrl) {
        debug("new task_ctrl fail\r\n");
        return NULL;
    }
    puts("creat_task 222 \r\n");
    
    memset(task_ctrl, 0, sizeof(TASK_CTRL));
    task_ctrl->stack_base = New(stack_size);
    if (!task_ctrl->stack_base) {
        debug("new stack fail\r\n");
        Delete(task_ctrl);
        return NULL;
    }
    
    puts("creat_task 333 \r\n");
    top = task_ctrl->stack_base + stack_size / sizeof(int);
    task_ctrl->prio = prio;
    printf("new task_ctrl task_ctrl->stack_base[%d] top[%d]\r\n", task_ctrl->stack_base, top);
        

    /* 2、伪造现场 */
    top -= 17;   /* 17*4byte用于保存17个寄存器 */
    task_stacks[task_count++] = (int)top; /* 记录栈顶的位置，用于下次恢复 */
    top[0] = task_ctrl;  //r0
    top[1] = param;  //r1
    top[2] = 0;  //r2
    top[3] = 0;  //r3
    top[4] = 0;  //r4
    top[5] = 0;  //r5
    top[6] = 0;  //r6
    top[7] = 0;  //r7
    top[8] = 0;  //r8
    top[9] = 0;  //r9
    top[10] = 0;  //r10
    top[11] = 0;  //r11
    top[12] = 0;  //r12
    top[13] = top + 17;  //sp
    top[14] = f;  //lr
    
    top[15] = f;  //任务入口
    top[16] = 0x50;  //psr开启中断设为user模式
    task_ctrl->stack = top;
    
    //top + 17*4 - 4096 = task_ctrl->stack_base
    printf("new task_ctrl task_ctrl->stack_base[%d] top[%d]\r\n", task_ctrl->stack_base, top);  

    /* 3、插到就绪链表尾部 */
    RdyList_TailInsert(task_ctrl);

    return task_ctrl;
}

/* 退出task */
void task_exit(TASK_CTRL *task_ctrl) {
    debug("task_c %d  task_ctrl[%d] [%d]  exit!!!!! \r\n",SysTickTime, task_ctrl ,task_ctrl->stack_base);
    Enter_Critical();
    RdyListRemove(task_ctrl);
    Delete(task_ctrl->stack_base);
    Delete(task_ctrl);
    Exit_Critical();
    debug("11111111111exit!!!!! \r\n");
    while(1);
    debug("2222222222exit!!!!! \r\n");
}

/* 启动task */
void start_task() {
    timer_init(SYSTIMECYL); /* SYSTIMECYL ms 产生异常中断 */
    while(1);
}

/* 就绪列表头部节点移至尾部，后续可采用环形链表优化 */
void RdyListHeadToTail(RDY_LIST* List)
{
    if (List->TaskNum >= 2) {
        List->HeadPoint->PrevPtr = List->TailPoint;  //原头节点前指针指向原尾结点
        List->TailPoint->NextPtr = List->HeadPoint;  //原尾结点后指针指向原头结点
        List->TailPoint = List->HeadPoint;           //链表尾指针指向原头结点
        List->HeadPoint->NextPtr->PrevPtr = NULL;    //原第2个节点前指针为空
        List->HeadPoint = List->HeadPoint->NextPtr;  //链表头指针指向原第2个结点
        List->TailPoint->NextPtr = NULL;             //原头节点后指针为空
    } return;
}

/*  获取最高优先级 */
unsigned char GetHighPrio(void) {
    unsigned char i = 0;
    while ((i < (sizeof(gPrioTbl) / sizeof(gPrioTbl[0]))) && (gPrioTbl[i] == 0)) ++i;
    return (i * sizeof(sizeof(gPrioTbl[0])) + LeadZeros(gPrioTbl[i]));
}

/* 时基列表更新 */
void TickListUpdate() {
    TASK_CTRL **TickListptr = &TickList[(++SysTickTime) % TICKLISTSIZE];
    TASK_CTRL *loop = *TickListptr;
    TASK_CTRL *next = NULL;
    while (loop != NULL && loop->dlySysTime <= SysTickTime) {
        next = loop->NextPtr;
        RdyList_TailInsert(loop);
        loop = next;
        *TickListptr = loop;
    } return;
}

void stack_debug(int *stack)
{
    int i = 0;
    for (i = 0; i < 32; ++i) {
        printf("[%d:%d] ", i, stack[i]);
    }printf("\r\n");
    return;
}

/* 更新上个task栈顶 & 更新下个task */
int saveandupdate_curstack(int *r0) {
    int bit = INTOFFSET;

    if (bit != 10) {
        //非定时器中断
        handle_irq_c(0);
        if (NULL != gpCurTaskCtrl)
            return r0;  //不触发调度
    } else {
        touchscreen_timer_irq();
    }

    /* 时基列表更新 */
    TickListUpdate();

    if (NULL != gpCurTaskCtrl) {
        /* 更新当前任务栈顶位置 */
        gpCurTaskCtrl->stack = r0;

        /* 任务轮询 */
        if (gpCurTaskCtrl->TaskState == TASK_RDY)
            RdyListHeadToTail(&gRdyList[gpCurTaskCtrl->prio]);
    }
    
    /* 从就绪列表中获取下一个最高优先级的任务 */
    gpCurTaskCtrl = gRdyList[GetHighPrio()].HeadPoint;
    sleepwait = 0;

    /* 清中断 */
    SRCPND = (1 << INTOFFSET);
    INTPND = (1 << INTOFFSET);

    return gpCurTaskCtrl->stack;
}

/* 时基列表插入节点 */
void TickListInsert(TASK_CTRL* pCurTaskCtrl, unsigned int time) {
    if (0 == time) return;
    
    TASK_CTRL **TickListptr = &TickList[(time + SysTickTime) % TICKLISTSIZE];
    TASK_CTRL *loop = *TickListptr;
    TASK_CTRL *last = *TickListptr;
    pCurTaskCtrl->dlySysTime = time + SysTickTime;
    pCurTaskCtrl->TaskState = TASK_TICK;

    /* TickList为排序过的单向链表，故无需对PrevPtr操作 */
    if (*TickListptr == NULL || (*TickListptr)->dlySysTime > pCurTaskCtrl->dlySysTime) {
        /* 头节点 */
        pCurTaskCtrl->NextPtr = *TickListptr;
        *TickListptr = pCurTaskCtrl;
        return;
    }

    /* 非头节点 */
    while (loop && loop->dlySysTime < pCurTaskCtrl->dlySysTime) {
        last = loop;
        loop = loop->NextPtr;
    }
    pCurTaskCtrl->NextPtr = last->NextPtr;
    last->NextPtr = pCurTaskCtrl;
    return;
}

/* 睡眠(time ms) */
void msleep(unsigned int time) {
    Enter_Critical();
    RdyListRemove(gpCurTaskCtrl);
    TickListInsert(gpCurTaskCtrl, time / SYSTIMECYL);
    Exit_Critical();
    //若退出临界区后立即被调度，sleepwait可能会死循环，需优化
    Sleepwait();
    return;
}

