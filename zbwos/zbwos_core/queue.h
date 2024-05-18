#ifndef __QUEUE_H_
#define __QUEUE_H_

#define QUEUE_MSG_POOL_MAX 64     //队列数量最大值

/* 消息体 
  msg --next--> msg --next--> msg
  msg <--prev-- msg <--prev-- msg
   |                           |
  tail                        head
*/
struct queue_msg {
 void* msg;
 struct queue_msg* next;   //下一个节点
 struct queue_msg* prev;   //上一个节点
};
typedef struct queue_msg QUEUE_MSG_S;

/* 消息队列管理 */
typedef struct {
 int id;     //队列id
 int size;   //消息大小
 QUEUE_MSG_S* head;   //队头
 QUEUE_MSG_S* tail;   //队尾
} QUEUE_S;

int queue_creat(int size);
int queue_destroy(int id);
int queue_send(int id, void *msg);
int queue_recv(int id, void *msg);

#endif
