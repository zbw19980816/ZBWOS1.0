#ifndef __SHELL_H_
#define __SHELL_H_

typedef void (*shellcmdproc)(char *cmd);

typedef struct {
    char *cmd;
    shellcmdproc fun;
} SHELL_TAB;

//文件接收命令(comfileget)解析
typedef struct {
    int filesize;       //文件大小
    char filename[64];  //文件名称
} SHELL_COMFILEGET_INFO;

void task_shell(void *param);

#endif
