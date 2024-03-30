#include "filesystem.h"
#include "stl.h"

#if 0
static FILESYSTEM_CTRL g_filesystem_ctrl;      //文件系统全局控制块

/* 初始化文件系统 */
void filesystem_init() {
    nand_read(NAND_BEGIN, &g_filesystem_ctrl, sizeof(g_filesystem_ctrl));  //读出文件树
    printf("----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    #if 1
    //printf("filesystem_init %d\r\n", g_systree[0].nextfile);

    /* 读测试 */
    int i, j;
    int buf[16] = {0};
    printf("[sizeof(g_filesystem_ctrl) / sizeof(buf): %d]\r\n", sizeof(g_filesystem_ctrl) / sizeof(buf));

    for (j = 0; j < sizeof(g_filesystem_ctrl) / sizeof(buf); ++j) {
        memset(buf, 0, sizeof(buf));
        nand_read(NAND_BEGIN + j * sizeof(buf), buf, sizeof(buf));
        printf("[addr: %d]", NAND_BEGIN + j * sizeof(buf));
        for (i = 0; i < sizeof(buf)/sizeof(int) ; ++i) {
            printf("[%d]", buf[i]);
        } printf("\r\n");
    }

    #endif
    return;
}

/* 文件树内容刷新nand flash */
void filesystem_sync() {
    Enter_Critical();
    nand_erase(NAND_BEGIN, ALIGN(sizeof(FILESYSTEM_CTRL), 128 * 1024));
    printf("222----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    
    nand_write(NAND_BEGIN, &g_filesystem_ctrl, sizeof(g_filesystem_ctrl));
    printf("333----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
        

#if 0
    int i = 0;
    unsigned char buf[32] = {0};
    nand_read(NAND_BEGIN, buf, sizeof(buf));
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("1 nand_read buf: ", buf[i]);
    }

    unsigned char *p = (unsigned char *)&g_filesystem_ctrl;
    for (i = 0; i < 32; ++i) {
        pri_byte("2 nand_read buf: ", p[i]);
    }
#endif
    
    Exit_Critical();
    return;
}

void prisys()
{
    NAND_MAP *pnandmap = &g_filesystem_ctrl.nandmap;
    int i,j;
    for (i = 0; i < sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]); ++i) {
        for (j = 0; j < sizeof(pnandmap->nand_128k[0]); ++j) {
            
            printf("find_freenandmap pnandmap->nand_128k[i][%d]\r\n", pnandmap->nand_128k[i]);
            if ((pnandmap->nand_128k[i] & (1<<j)) == 0) {
                
                printf("find_freenandmap [%d][%d][%d]\r\n", i, j, 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j));
                return 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j);
            }
        }
    }

}
/* 清空文件树（nand 15M~16M） */
void filesystem_erase() {
    Enter_Critical();
    memset(&g_filesystem_ctrl, 0, sizeof(g_filesystem_ctrl));
    //prisys();
    printf("111----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    
    filesystem_sync();
    printf("-----sizeof(g_filesystem_ctrl) [%d]\r\n", sizeof(g_filesystem_ctrl));
    filesystem_init();
    //prisys();
    Exit_Critical();
    return;
}

/* 获取一个空闲nand flash首地址 */
int filesystem_nandget() {
    return 0;
}

/* 路径解析结果释放 */
void list_free(PATH_LIST *list) {
    PATH_LIST *tmp = NULL;

    while (NULL != list) {
        tmp = list->next;
        delete(list);
        list = tmp;
    }

    return;
}

/* 路径解析 */
PATH_LIST* prase_path(char *dirpath) {
    PATH_LIST *ret = NULL;
    PATH_LIST *last = NULL;
    char *find = NULL;
    
    if (dirpath[0] != '/') {
        return ret;
    }
    
    ++dirpath;

    while (*dirpath != '\0') {
        find = strstr(dirpath, "/");
        if (find == dirpath) {
            /* 过滤连续的"//" */
            list_free(ret);
            return NULL;
        }
        
        if (ret == NULL) {
            ret = new(sizeof(PATH_LIST));
            last = ret;;
        } else {
            last->next = new(sizeof(PATH_LIST));
            last = last->next;
        }
        
        memset(last, 0, sizeof(PATH_LIST));
        if (NULL != find) {
            memcpy(last->name, dirpath, find - dirpath);
            dirpath = find + 1;
        } else {
            strcpy(last->name, dirpath);
            break;
        }
    }
    return ret;
}

/* 找到一个空闲的文件数节点 */
SYS_TREE* find_freetreenode() {
    printf("find_freetreenode \r\n");
    int i = 0;
    for (i = 0; i < sizeof(g_filesystem_ctrl.systree) / sizeof(g_filesystem_ctrl.systree[0]); ++i) {
        if (g_filesystem_ctrl.systree[i].type == TREENODE_NONE) {
            return &g_filesystem_ctrl.systree[i];
        }
    }printf("find_freetreenode return NULL ?????\r\n");
    return NULL;
}


/* 强制创建目录 */
void make_dir_force(SYS_TREE **treeloop, PATH_LIST *praseloop) {
    while (praseloop != NULL) {
        printf("find_freetreenode >>>>>>>>>>111 %d\r\n", (*treeloop));
        (*treeloop) = find_freetreenode();
        printf("find_freetreenode <<<<<<<<<222 %d\r\n", (*treeloop));
        if ((*treeloop) == NULL) {
            printf("make_dir_force fail\r\n");
            return;
        }
        
        (*treeloop)->type = TREENODE_DIR;
        strcpy((*treeloop)->name, praseloop->name);
        treeloop = &((*treeloop)->nextdir);
        praseloop = praseloop->next;
    }
    return;
}

/* 创建目录 PATH_LIST *prase 文件树字串解析结果链表 */
int make_dirbypathlist(PATH_LIST *prase) {
    PATH_LIST *praseloop = prase;
    SYS_TREE *treeloop = g_filesystem_ctrl.root;
    SYS_TREE **mkdir = &g_filesystem_ctrl.root;

    /* 判断目录是否有已存在的层级 */
    while (treeloop != NULL) {
        if (0 == strcmp(treeloop->name, praseloop->name)) {
            if (treeloop->type != TREENODE_DIR) {
                /* 有同命名的文件存在，直接创建失败 */
                printf("[make_dirbypathlist] illegal path\r\n");
                return -1;
            }
            
            mkdir = &(treeloop->nextfile);
            treeloop = treeloop->nextdir;
            praseloop = praseloop->next;
            if (praseloop == NULL) {
                /* 路径本来就存在 */
                return 0;
            }
        } else {
            mkdir = &(treeloop->nextfile);
            treeloop = treeloop->nextfile;
        }
    }
    
    /* 在treeloop下创建praseloop */
    make_dir_force(mkdir, praseloop);
    return 0;
}

int find_freenandmap(int filesize)
{
    int i = 0;
    int j = 0;
    NAND_MAP *pnandmap = &g_filesystem_ctrl.nandmap;
    if (filesize <= 128 * 1024) filesize = 128 * 1024;
    else if (filesize <= 512 * 1024) filesize = 512 * 1024;
    else if (filesize <= 1 * 1024 * 1024) filesize = 1 * 1024 * 1024;
    else if (filesize <= 4 * 1024 * 1024) filesize = 4 * 1024 * 1024;
    else if (filesize <= 8 * 1024 * 1024) filesize = 8 * 1024 * 1024;
    else if (filesize <= 16 * 1024 * 1024) filesize = 16 * 1024 * 1024;
    else if (filesize <= 32 * 1024 * 1024) filesize = 32 * 1024 * 1024;
    else return NULL;
    printf("find_freenandmap filesize[%d]\r\n", filesize);
    switch (filesize) {
        case 128 * 1024: {
            printf("find_freenandmap sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0])[%d]\r\n", sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]));
            for (i = 0; i < sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]); ++i) {
                for (j = 0; j < sizeof(pnandmap->nand_128k[0]); ++j) {
                    
                    printf("find_freenandmap pnandmap->nand_128k[i][%d]\r\n", pnandmap->nand_128k[i]);
                    if ((pnandmap->nand_128k[i] & (1<<j)) == 0) {
                        
                        printf("find_freenandmap [%d][%d][%d]\r\n", i, j, 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j));
                        return 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j);
                    }
                }
            }
        }
       /* case 128 * 1024:
                for (i = 0; i < sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]); ++i) {
                    for (j = 0; j < 32; ++j) {
                        if ((pnandmap->nand_128k[i] & (1<<j)) == 0) {
                            return 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j);
                        }
                    }
                }*/
    }
}


/* 创建文件 */
SYS_TREE* make_filebypathlist(PATH_LIST *prase, int filesize) {
    PATH_LIST *praseloop = prase;
    SYS_TREE *treeloop = g_filesystem_ctrl.root;
    SYS_TREE **mkfile = &g_filesystem_ctrl.root;
    
    while (treeloop != NULL && praseloop->next != NULL) {
        //printf("treeloop->name[%s], praseloop->name[%s]\r\n",treeloop->name, praseloop->name);
        if (0 == strcmp(treeloop->name, praseloop->name)) {
            praseloop = praseloop->next;
            if (praseloop->next == NULL) {
                break;
            }
            treeloop = treeloop->nextdir;
        } else {
            treeloop = treeloop->nextfile;
        }
    }
    
    //printf("000 treeloop[%d], treeloop->nextfile[%d], &(treeloop->nextfile)[%d]\r\n", 
    //    treeloop, treeloop->nextfile, &(treeloop->nextfile));
    mkfile = &(treeloop->nextdir);
    treeloop = treeloop->nextdir;
    //printf("111 mkfile[%d], (*mkfile)[%d]\r\n", mkfile, (*mkfile));
    while (treeloop != NULL) {
        if (0 == strcmp(treeloop->name, praseloop->name)) {
            printf("file has already exist\r\n");
            return treeloop;
        }
        mkfile = &(treeloop->nextfile);
        treeloop = treeloop->nextfile;
    }
    //printf("222 mkfile[%d], (*mkfile)[%d]\r\n", mkfile, (*mkfile));
    
    //printf("find_freetreenode >>>>>>>>>> %d\r\n", (*mkfile));
    (*mkfile) = find_freetreenode();
    //printf("find_freetreenode <<<<<< %d\r\n", (*mkfile));
    if ((*mkfile) == NULL) {
        printf("find_freetreenode fail\r\n");
        return;
    }
    
    (*mkfile)->type = TREENODE_FILE;
    //printf("5555\r\n");
    //return NULL;
    strcpy((*mkfile)->name, praseloop->name);
    (*mkfile)->size = filesize;
    (*mkfile)->nandaddr = find_freenandmap(filesize);
    
    return *mkfile;
}


/* 创建目录 char *dirpath 目录绝对路径 */
int make_dir(char *dirpath) {
    PATH_LIST *prase = prase_path(dirpath);
    int ret = 0;
    
    if (prase == NULL) {
        printf("[make_dir] illegal path[%s]\r\n", dirpath);
        return -1;
    }

    ret = make_dirbypathlist(prase);

    
    list_free(prase);
    return ret;
}

/* 创建文件; char *filepath 文件绝对路径, int fizesize 文件大小 */
SYS_TREE *make_file(char *filepath, int fizesize) {
    PATH_LIST *prase = prase_path(filepath);
    PATH_LIST *prase_loop = prase;
    PATH_LIST *prase_filename;
    SYS_TREE *treeret = NULL;
    int ret = 0;
    
    if (prase == NULL) {
        printf("prase_path fail\r\n");
        return NULL;
    }
    
    if (prase_loop->next != NULL) {
        //将链表中最后一项(文件)拆离
        while (prase_loop->next->next != NULL) {
            prase_loop = prase_loop->next;
        }
        prase_filename = prase_loop->next;
        prase_loop->next = NULL;

        /* 创建路径 */
        ret = make_dirbypathlist(prase);
        if (ret == -1) {
            list_free(prase);
            return NULL;
        }

        /* 恢复链表 */
        prase_loop = prase;
        while (prase_loop->next != NULL) {
            prase_loop = prase_loop->next;
        }
        prase_loop->next = prase_filename;
    }

    /* 创建文件 */
    treeret = make_filebypathlist(prase, fizesize);
    list_free(prase);
    return treeret;
}

/* 写文件; SYS_TREE *fd 文件树节点, int writesize 写入大小, int writeoffset 写入偏移, char *buf 写入数据 */
int write_file(SYS_TREE *fd, int writesize, int writeoffset, char *buf) {
    printf("write_file begin, fd->nandaddr[%d], writesize[%d], writeoffset[%d]\r\n",fd->nandaddr, writesize, writeoffset);
    
    nand_erase(fd->nandaddr + writeoffset, writesize);        //擦除
    nand_write(fd->nandaddr + writeoffset, buf, writesize);   //写入

    /* 读测试 */
        int i = 0;
        unsigned char Buf[32] = {0};
        nand_read(fd->nandaddr + writeoffset, Buf, sizeof(Buf));
        for (i = 0; i < sizeof(Buf); ++i) {
            pri_byte("1 nand_read buf: ", Buf[i]);  //.bin的前16个byte
        }

        for (i = 0; i < 16; ++i) {
            pri_byte("1 nand_write buf: ", buf[i]);  //.bin的前16个byte
        }

    return 0;
}

/* 打开一个文件 */
SYS_TREE* open_file(char *path) {
    if (NULL == path) {
        return NULL;
    }
    printf("open_file >>> %s \r\n", path);
    PATH_LIST *prase = prase_path(path);
    printf("open_file >>> \r\n");

    SYS_TREE *treeloop = g_filesystem_ctrl.root;
    while (NULL != prase) {
        printf("treeloop->name[%s], prase->name[%s] \r\n", treeloop->name, prase->name);
        if (0 == strcmp(treeloop->name, prase->name)) {
            if (NULL == prase->next) {
                return treeloop;
            }

            prase = prase->next;
            treeloop = treeloop->nextdir;
        } else {
            treeloop = treeloop->nextfile;
        }
    }

    return NULL;
}

void print_tree(char *path) {
    SYS_TREE *treeloop = g_filesystem_ctrl.root;
    PATH_LIST *prase = prase_path(path);
    PATH_LIST *loop = prase;
   // printf("[path] %s  prase[%d]\r\n", path, prase);
   printf("treeloop [%d]\r\n", treeloop);
    if (NULL == prase) {
        PRI:
        while (treeloop != NULL) {
            if (TREENODE_FILE == treeloop->type) printf("[file] %s\r\n", treeloop->name);
            else if (TREENODE_DIR == treeloop->type) printf("[dir]  %s\r\n", treeloop->name);
            treeloop = treeloop->nextfile;
        } 
        
        list_free(prase);
        return;
    }
    
    while (treeloop != NULL && loop!= NULL) {
       // printf("treeloop->name %s, loop->name %s\r\n",treeloop->name, loop->name);
        if (0 == strcmp(treeloop->name, loop->name)) {
            loop = loop->next;
            if (TREENODE_DIR == treeloop->type) treeloop = treeloop->nextdir;
            else {
                printf("tree fail\r\n");
                list_free(prase);
                return;
            }
        } else treeloop = treeloop->nextfile;
    }
    goto PRI;
}

#if 0  //显示完整文件树，后续实现
/* 遍历文件树内容 */
void ergodic_tree(QUEUE *q) {
    SYS_TREE *treeloop = NULL;
    QUEUE new_q;
    
    while (is_que_empty(q)) {
        treeloop = que_pop(q);
        while (treeloop != NULL) {
            printf("%s ", treeloop->name);
            if (treeloop->nextdir != NULL) {
                que_push(new_q, treeloop);
            }
            treeloop = treeloop->nextfile;
        }
    }
    printf("\r\n");

    return;
}

/* 显示文件树内容 */
void print_tree() {
    SYS_TREE *treeloop = g_filesystem_ctrl.root;
    QUEUE q;
    
    if (treeloop == NULL) {
        return;
    }

    q.data = treeloop;
    q.next = NULL;
    ergodic_tree(&q);
}

typedef struct queue {
    void *data;
    struct queue next;
} QUEUE;
#endif
#endif

static FILESYSTEM_CTRL g_filesystem_ctrl;      //文件系统全局控制块

/* 初始化文件系统 */
void filesystem_init() {
    int i = 0;
    nand_read(NAND_BEGIN, &g_filesystem_ctrl, sizeof(g_filesystem_ctrl));  //读出文件树
    printf("filesystem tree: \r\n");
    for (i = 0; i < TREE_FILE_MAX; ++i) {
        if (g_filesystem_ctrl.systree[i].size > 0) {
            printf ("name[%s] size[%d] nandaddr[%d] \r\n", g_filesystem_ctrl.systree[i].name, g_filesystem_ctrl.systree[i].size, g_filesystem_ctrl.systree[i].nandaddr);
        }
    }
    //printf("----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    #if 0
    //printf("filesystem_init %d\r\n", g_systree[0].nextfile);

    /* 读测试 */
    int i, j;
    int buf[16] = {0};
    printf("[sizeof(g_filesystem_ctrl) / sizeof(buf): %d]\r\n", sizeof(g_filesystem_ctrl) / sizeof(buf));

    for (j = 0; j < sizeof(g_filesystem_ctrl) / sizeof(buf); ++j) {
        memset(buf, 0, sizeof(buf));
        nand_read(NAND_BEGIN + j * sizeof(buf), buf, sizeof(buf));
        printf("[addr: %d]", NAND_BEGIN + j * sizeof(buf));
        for (i = 0; i < sizeof(buf)/sizeof(int) ; ++i) {
            printf("[%d]", buf[i]);
        } printf("\r\n");
    }

    #endif
    return;
}

/* 文件树内容刷新nand flash */
void filesystem_sync() {
    Enter_Critical();
    nand_erase(NAND_BEGIN, ALIGN(sizeof(g_filesystem_ctrl), 128 * 1024));
    //printf("222----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    
    nand_write(NAND_BEGIN, &g_filesystem_ctrl, sizeof(g_filesystem_ctrl));
    //printf("333----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
        

#if 0
    int i = 0;
    unsigned char buf[32] = {0};
    nand_read(NAND_BEGIN, buf, sizeof(buf));
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("1 nand_read buf: ", buf[i]);
    }

    unsigned char *p = (unsigned char *)&g_filesystem_ctrl;
    for (i = 0; i < 32; ++i) {
        pri_byte("2 nand_read buf: ", p[i]);
    }
#endif
    
    Exit_Critical();
    return;
}

/* 清空文件树（nand 15M~16M） */
void filesystem_erase() {
    Enter_Critical();
    memset(&g_filesystem_ctrl, 0, sizeof(g_filesystem_ctrl));
    //prisys();
    //printf("111----filesystem_init [%d][%d] \r\n", g_filesystem_ctrl.root, g_filesystem_ctrl.nandmap.nand_128k[0]);
    
    filesystem_sync();
    printf("-----sizeof(g_filesystem_ctrl) [%d]\r\n", sizeof(g_filesystem_ctrl));
    filesystem_init();
    //prisys();
    Exit_Critical();
    return;
}

/* 查找可用nand块 */
int filesystem_find_nandmap(int filesize) {
    int i = 0;
    int j = 0;
    NAND_MAP *pnandmap = &g_filesystem_ctrl.nandmap;
    
    if (filesize <= NAND_128K_SIZE) filesize = NAND_128K_SIZE;
    else if (filesize <= NAND_512K_SIZE) filesize = NAND_512K_SIZE;
    else if (filesize <= NAND_1M_SIZE) filesize = NAND_1M_SIZE;
    else if (filesize <= NAND_4M_SIZE) filesize = NAND_4M_SIZE;
    else if (filesize <= NAND_8M_SIZE) filesize = NAND_8M_SIZE;
    else if (filesize <= NAND_16M_SIZE) filesize = NAND_16M_SIZE;
    else if (filesize <= NAND_32M_SIZE) filesize = NAND_32M_SIZE;
    else return -1;
    
    switch (filesize) {
        case 128 * 1024: {
            //printf("find_freenandmap sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0])[%d]\r\n", sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]));
            for (i = 0; i < NAND_128K_NUM; ++i) {
                    if ((pnandmap->nand_128k[i / sizeof(pnandmap->nand_128k[0])] & (1<<(i % 32))) == 0) {
                        pnandmap->nand_128k[i / sizeof(pnandmap->nand_128k[0])] |= (1<<(i % 32));  //标记为使用
                        return NAND_128K_BASE + NAND_128K_SIZE * i;  //存储块首地址
                    }
            }
            break;
        }
       /* case 128 * 1024:
                for (i = 0; i < sizeof(pnandmap->nand_128k) / sizeof(pnandmap->nand_128k[0]); ++i) {
                    for (j = 0; j < 32; ++j) {
                        if ((pnandmap->nand_128k[i] & (1<<j)) == 0) {
                            return 16 * 1024 * 1024 + 128 * 1024 * (i * 32 + j);
                        }
                    }
                }*/
    }

    return -1;
}

/* 查找文件树缓存可用节点 */
SYS_TREE* filesystem_find_systree() {
    SYS_TREE *systree = g_filesystem_ctrl.systree;

    for ( ; systree <= &g_filesystem_ctrl.systree[TREE_FILE_MAX - 1]; ++systree)
        if (0 == systree->size) return systree;
    return NULL;
}

/* 打开一个已存在的文件 */
SYS_TREE* filesystem_open_file(char *path) {
    SYS_TREE *treeloop = g_filesystem_ctrl.systree;
    
    if (NULL == path) return NULL;
    for ( ; treeloop <= &g_filesystem_ctrl.systree[TREE_FILE_MAX - 1]; ++treeloop)
        if (0 == strcmp(treeloop->name, path)) return treeloop;
    return NULL;
}

/* 创建文件; char *filepath 文件名称, int fizesize 文件大小 */
SYS_TREE *filesystem_make_file(char *filename, int filesize) {
    SYS_TREE* sys_treenode = NULL;
    int baseaddress = -1;
    
    sys_treenode = filesystem_open_file(filename);
    if (NULL != sys_treenode) {
        //文件已存在，先删除
        memset(sys_treenode, 0, sizeof(*sys_treenode));
    }

    //查找filesystem可用node
    sys_treenode = filesystem_find_systree();
    if (NULL == sys_treenode) {
        printf("filesystem_find_systree fail\r\n");
        return NULL;
    }
    
    //查找nand可用块
    baseaddress = filesystem_find_nandmap(filesize);
    if (-1 == baseaddress) {
        printf("filesystem_find_nandmap fail\r\n");
        return NULL;
    }
    
    strcpy(sys_treenode->name, filename);
    sys_treenode->nandaddr = baseaddress;
    sys_treenode->size = filesize;
    
    return sys_treenode;
}

/* 写文件; SYS_TREE *fd 文件树节点, int writesize 写入大小, int writeoffset 写入偏移, char *buf 写入数据 */
int filesystem_write_file(SYS_TREE *fd, int writesize, int writeoffset, char *buf) {
    printf("write_file begin, fd->nandaddr[%d], writesize[%d], writeoffset[%d]\r\n",fd->nandaddr, writesize, writeoffset);
    
    nand_erase(fd->nandaddr + writeoffset, ALIGN(writesize, 128 * 1024));
    nand_write(fd->nandaddr + writeoffset, buf, writesize);

    /* 读测试 */
    #if 0
        int i = 0;
        unsigned char Buf[32] = {0};
        nand_read(fd->nandaddr + writeoffset, Buf, sizeof(Buf));
        for (i = 0; i < sizeof(Buf); ++i) {
            pri_byte("1 nand_read buf: ", Buf[i]);  //.bin的前16个byte
        }

        for (i = 0; i < 16; ++i) {
            pri_byte("1 nand_write buf: ", buf[i]);  //.bin的前16个byte
        }
#endif
    return 0;
}



/* 读文件 */
int filesystem_read_file(SYS_TREE* file, char *buf, int size) {
    if (NULL == file) return -1;
    if (file->size <= size) {
        nand_read(file->nandaddr, buf, file->size);
        return file->size;
    } else {
        nand_read(file->nandaddr, buf, size);
        return size;
    }
}

/* 输出所有文件 */
void filesystem_print_tree() {
    SYS_TREE *treeloop = g_filesystem_ctrl.systree;

    for ( ; treeloop <= &g_filesystem_ctrl.systree[TREE_FILE_MAX - 1]; ++treeloop)
        if (treeloop->size > 0) printf("name: %s size: %d\r\n", treeloop->name, treeloop->size);
    return;
}


