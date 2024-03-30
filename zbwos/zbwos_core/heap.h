#ifndef __HEAP_H_
#define __HEAP_H_

#define HEAPSIZE (1024 * 1024)    //堆大小：sizeof(unsigned int) * 1024 * 1024 = 4M

/* 内存链表控制结构 */
typedef struct heapctrl {
    unsigned int size;      //内存大小
    struct heapctrl *next;  //下个内存空闲节点
} HEAPCTRL;

/* 内存使用信息 */
typedef struct {
    unsigned int all;       //总内存（byte）
    unsigned int free;      //剩余内存（byte）
} HEAPMEMINFO;

void* New(unsigned int size);
void Delete(void *addr);
void getheapmeminfo(HEAPMEMINFO *heapmeminfo);

#endif
