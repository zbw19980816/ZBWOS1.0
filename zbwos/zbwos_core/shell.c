#include "shell.h"
#include "stl.h"
#include "heap.h"
#include "filesystem.h"

/* 回车 */
void enterproc(char *cmd) {
    return;
}

/* 开启debug */
void opendebug(char *cmd) {
    setdebug(1);
    return;
}

/* 关闭debug */
void closedebug(char *cmd) {
    setdebug(0);
    return;
}

/* 获取内存池使用情况 */
void getheapmem(char *cmd) {
    HEAPMEMINFO heapmeminfo;
    getheapmeminfo(&heapmeminfo);
    printf("all:%d   used:%d   free:%d\r\n", 
        heapmeminfo.all, heapmeminfo.all - heapmeminfo.free, heapmeminfo.free);
    return;
}

/* 重启 */
void reboot(char *cmd) {
    printf("zbwos reboot now ...\r\n");
    //filesystem_sync();
    _reboot();
    return;
}

/* 获取文件信息 */
int getfileinfo(char *cmd, SHELL_COMFILEGET_INFO *fileinfo) {
    char *find = cmd;
    char *name_base;
    char *name_end;

    find = strstr(cmd, "comfileget");
    name_base = find + strlen("comfileget");
    while (*name_base == ' ') name_base++;
    name_end = name_base;
    while (*name_end != ' ') name_end++;
    strncpy(fileinfo->filename, name_base, name_end - name_base);
    while (*name_end == ' ') name_end++;
    fileinfo->filesize = _atoi(name_end);

    return 0;
}

/* 获取副指令 */
char* getnextcmd(char *cmd) {
    char *find = cmd;
    while ((find = strstr(cmd, " ")) && (find != NULL)) { cmd = find + 1; }
    
    return cmd;
}

/* 获取串口发送文件(格式：comfileget 文件名 文件大小) */
void comfileget(char *cmd) {
    int i = 0;
    int nandaddr;                       //文件存储地址
    SHELL_COMFILEGET_INFO fileinfo;
    char *recbuf = NULL;                //串口缓存

    memset(&fileinfo, 0, sizeof(fileinfo));
    getfileinfo(cmd, &fileinfo);    //获取传输文件信息

    printf("========== fileget begin, name[%s], size[%d] ==========\r\n", fileinfo.filename, fileinfo.filesize);
    if (fileinfo.filesize <= 0) {
        printf("========== fileget fail ==========\r\n");
        return;
    }

    recbuf = New(1024 * 128);
    if (NULL == recbuf) {
        printf("========== fileget fail ==========\r\n");
        return;
    }

    /* 创建接收路径 */
    //make_dir("/comrecv");
    
    /* 创建接收文件 */
    SYS_TREE *fd = filesystem_make_file(fileinfo.filename, fileinfo.filesize);
    //return;
    
    printf("fd->name[%s], fd->nandaddr[%d], fd->size[%d]\r\n", fd->name, fd->nandaddr, fd->size);
    printf("========== fileget begin ==========\r\n");
    int writeoffset = 0;
    getchar();  //串口文件传输工具第一个字节（0x10）过滤
    while (fileinfo.filesize-- > 0) {
        recbuf[i++] = getchar();
        //if (i%100 == 0)
        //printf("%d : %d\r\n", i, recbuf[i-1]);
        if (i == 1024 * 128) {
            //串口缓存用满，先写flash
            filesystem_write_file(fd, 1024 * 128, 1024 * 128 * writeoffset, recbuf);
            writeoffset++;
            i = 0;
        }
    }
    //recbuf[i] = 0;
    //printf("recbuf[%s]\r\n", recbuf);
    
    filesystem_write_file(fd, 1024 * 128, 1024 * 128 * writeoffset, recbuf);
    Delete(recbuf);
    filesystem_sync();
    return;
}

/* 清空文件树 */
void flashclear(char *cmd) {
    filesystem_erase();
    printf("flashclear finish\r\n");
    return;
}

/* 显示当前路径下所有文件 */
void tree(char *cmd) {
    filesystem_print_tree();
    return;
}

/* 创建文件 */
void mkfile(char *cmd) {
    filesystem_make_file(getnextcmd(cmd));
    return;
}

/* 文件树刷nand flash */
void sync(char *cmd) {
    filesystem_sync();
    return;
}

/* 查看文件二进制 */
void binary(char *cmd) {
    char *find = cmd;
    char *name_base;

    find = strstr(cmd, "binary");
    name_base = find + strlen("binary");
    while (*name_base == ' ') name_base++;

    filesystem_cat_binary(name_base);

    return;
}

/* 删除文件 */
void rm(char *cmd) {
    char *find = cmd;
    char *name_base;

    find = strstr(cmd, "rm");
    name_base = find + strlen("rm");
    while (*name_base == ' ') name_base++;

    filesystem_remove_file(name_base);

    return;
}

/* shell指令表 */
SHELL_TAB cmdtable[] = {
    /*  指令            回调   */
    {"reboot",      reboot},        /* 重启 */
    {"opendebug",   opendebug},     /* 开启debug */
    {"q",           closedebug},    /* 关闭debug */
    {"free",        getheapmem},    /* 获取内存池使用情况 */
    {"comfileget",  comfileget},    /* 获取串口发送文件 */
    {"flashclear",  flashclear},    /* 清空文件树 */
    {"tree",        tree},          /* 显示文件树 */
    {"mkfile",      mkfile},        /* 创建文件 */
    {"rm",          rm},            /* 删除文件 */
    //{"mkdir",       mkdir},         /* 创建目录 */
    {"sync",        sync},          /* 文件树刷nand flash */
    {"binary",      binary},        /* 查看文件二进制 */
    {"\n",          enterproc},     /* 回车 */
};

/* shell指令处理 */
void shellproc(char *cmd) {
    int i = 0;
    for (i = 0; i < (sizeof(cmdtable) / sizeof(cmdtable[0])); ++i) {
        if (cmd == strstr(cmd, cmdtable[i].cmd)) {
            cmdtable[i].fun(cmd);
            break;
        }
    }
    if (i == (sizeof(cmdtable) / sizeof(cmdtable[0]))) printf("%s: command not found\r\n", cmd);
    return;
}

/* 用于和user交互的task(目前只支持串口) */
void task_shell(void *param) {
    char get = 0;
    char buf[256] = {0};
    char *bufptr = NULL;
    while (1) {
        bufptr = buf;
        puts("# ");
        while (1) {
            get = getchar();
            if (get == '\r' || get == '\n') {
                puts("\r\n");
                if (bufptr == buf) {
                    *(bufptr++) = '\n';
                }
                *(bufptr++) = '\0';
                break;
            } else {
                putchar(get);
                *(bufptr++) = get;
            }
        }
        
        Enter_Critical();
        shellproc(buf);
        Exit_Critical();
    }
    
    return;
}

