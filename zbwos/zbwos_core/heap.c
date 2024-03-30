#include "heap.h"
#include "stl.h"

static HEAPCTRL heaphead;

/* 堆内存初始化 */
static void initheap() {
    unsigned int *bss_end = get_bss_end();
    unsigned int *uboot_start = 0x33f80000;
    HEAPCTRL *heappoint = (HEAPCTRL *)bss_end;
    
    heappoint->next = NULL;
    heappoint->size = (uboot_start - bss_end) * sizeof(unsigned int);
    heaphead.next = heappoint;
    heaphead.size = 0;
    
    return;
}

/* 堆内存申请 */
void* New(unsigned int size) {
    Enter_Critical();
    static char init = 0;
    HEAPCTRL *ret = NULL;
    HEAPCTRL *heappoint = NULL;

    if (0 == init) {
        initheap();
        init = 1;
    }
    
    size = ALIGN(size, 4);  //size非4对齐存在硬件异常问题
    
    heappoint = &heaphead;
    while (NULL != heappoint->next && heappoint->next->size < size + sizeof(HEAPCTRL)) {
        heappoint = heappoint->next;
    }

    if (NULL == heappoint->next) {
        Exit_Critical();
        return NULL;
    }

    /* 申请成功(首地址：heappoint->next + sizeof(HEAPCTRL)) */
    ret = heappoint->next;

    if (ret->size - (size + sizeof(HEAPCTRL)) >= sizeof(HEAPCTRL)) {
        heappoint->next = (HEAPCTRL *)((char *)ret + size + sizeof(HEAPCTRL));
        heappoint->next->next = ret->next;
        heappoint->next->size = ret->size - (size + sizeof(HEAPCTRL));
        ret->size = size + sizeof(HEAPCTRL);
    } else {
        heappoint->next = ret->next;
    }
    
    ret->next = NULL;
    Exit_Critical();

    return (char *)ret + sizeof(HEAPCTRL);
}

/* 堆内存回收 */
void Delete(void *addr) {
    Enter_Critical();
    HEAPCTRL *del = (char *)addr - sizeof(HEAPCTRL);
    HEAPCTRL *find = &heaphead;
    
    if (find == NULL) {
        find->next = del;
        Exit_Critical();
        return;
    }
    
    while (find->next != NULL && del > find->next) {
        find = find->next;
    }

    /* 在find和find->next中间插入del */
    if (((char *)find + find->size) == del) {
        find->size += del->size;
    } else {
        del->next = find->next;
        find->next = del;
        find = del;
    }

    if ((find->next != NULL) && ((char *)find + find->size) == find->next) {
        find->size += find->next->size;
        find->next = find->next->next;
    }
    
    Exit_Critical();
    return;
}

/* 获取内存池信息 */
void getheapmeminfo(HEAPMEMINFO *heapmeminfo) {
    Enter_Critical();
    unsigned int *bss_end = get_bss_end();
    unsigned int *uboot_start = 0x33f80000;
    HEAPCTRL *heappoint = &heaphead;
    
    heapmeminfo->all = (uboot_start - bss_end) * sizeof(unsigned int);
    heapmeminfo->free = 0;
    while (heappoint != NULL) {
        heapmeminfo->free += heappoint->size;
        heappoint = heappoint->next;
    }
    Exit_Critical();
    return;
}
