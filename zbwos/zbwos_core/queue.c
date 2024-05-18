
/*
 *  消息队列
 */
 
#include "stl.h"
#include "queue.h"


QUEUE_S queue_pool[QUEUE_MSG_POOL_MAX];    //队列缓存池信息

/* 创建一个队列(返回一个非负id，若创建失败，则返回-1) */
int queue_creat(int size) {
    int i;

    if (size <= 0)
        return -1;
    
    for (i = 0; i < sizeof(queue_pool) / sizeof(queue_pool[0]); ++i) {
        if (!queue_pool[i].size) {
            queue_pool[i].size = size;
            queue_pool[i].id = i;
            return queue_pool[i].id;
        }
    }

    return -1;
}

/* 销毁一个队列 */
int queue_destroy(int id) {
    QUEUE_MSG_S* dsty = queue_pool[id].head;
    QUEUE_MSG_S* prev = NULL;
    
    if (id >= QUEUE_MSG_POOL_MAX || id < 0)
        return -1;

    //初始化队列池节点
    memset(&queue_pool[id], 0, sizeof(queue_pool[id]));

    //从队头开始销毁资源
    while (dsty) {
        prev = dsty->prev;
        Delete(dsty->msg);
        Delete(dsty);
        dsty = prev;
    }

    return 0;
}

/* 发送 */
int queue_send(int id, void *msg) {
    QUEUE_MSG_S *pmsg;

    if (!msg || id >= QUEUE_MSG_POOL_MAX || id < 0)
        return -1;

    pmsg = (QUEUE_MSG_S *)New(sizeof(QUEUE_MSG_S));
    if (!pmsg)
        return -1;

    pmsg->msg = New(queue_pool[id].size);
    if (!pmsg->msg) {
        Delete(pmsg);
        return -1;
    }

    memcpy(pmsg->msg, msg, queue_pool[id].size);
    pmsg->next = queue_pool[id].tail;
    pmsg->prev = NULL;

    if (queue_pool[id].tail)
        queue_pool[id].tail->prev = pmsg;

    queue_pool[id].tail = pmsg;

    if (!queue_pool[id].head)
        queue_pool[id].head = pmsg;   //发送第一条消息

    return 0;
}

/* 接收 */
int queue_recv(int id, void *msg) {
    if (!queue_pool[id].head)
        return -1;   //没有消息

    if (!msg || id >= QUEUE_MSG_POOL_MAX || id < 0)
        return -1;

    memcpy(msg, queue_pool[id].head->msg, queue_pool[id].size);
    Delete(queue_pool[id].head->msg);
    Delete(queue_pool[id].head);

    queue_pool[id].head = queue_pool[id].head->prev;
    if (queue_pool[id].head)
        queue_pool[id].head->next = NULL;
    else
        queue_pool[id].tail = NULL;   //取走最后一条消息

    return 0;
}


