#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__
#if 0
/* nandflash容量：256M 
0~15M：存放bin文件
15M~16M：存放文件系统控制块
16M~256M：存放文件
*/

#define NAND_BEGIN      (1024 * 1024 * 15)          //文件系统起始地址
#define NAND_END        (1024 * 1024 * 256)         //文件系统结束地址
#define NAND_TREE_SIZE  (1024 * 1024)               //文件系统控制块分配大小，实际大小为sizeof(FILESYSTEM_CTRL),实际大小必须小于分配大小
#define TREE_FILE_MAX   (1024)                      //文件最大数量

/* 文件数节点类型 */
enum TREENODE_TYPE {
    TREENODE_NONE = 0,   //未使用
    TREENODE_FILE,       //文件
    TREENODE_DIR,        //目录
    
};

/* 文件树 */
typedef struct sys_tree {
    struct sys_tree *nextfile;      //同一层级的下一个文件
    struct sys_tree *nextdir;       //该目录下的第一个文件(目录类型特有)
    char name[32];                  //文件名
    int size;                       //文件大小
    int nandaddr;                   //nand存储首地址
    char type;                      //类型（TREENODE_TYPE）
    char tmp[3];                    //预留
} SYS_TREE;

/* nand使用映射表 */
typedef struct {
    /*                       存储块大小        存储块数量        存储块总大小         存储块区间 */
    int nand_32M;        //      32M     *     1      =      32M       (224M~255M)
    int nand_16M;        //      16M     *     2      =      32M       (192M~223M)
    int nand_8M;         //      8M      *     4      =      32M       (160M~191M)
    int nand_4M;         //      4M      *     8      =      32M       (128M~159M)
    int nand_1M;         //      1M      *     32     =      32M       (96M~127M)
    int nand_512k[2];    //      512k    *     64     =      32M       (64M~95M)
    int nand_128k[12];   //      128k    *     384    =      48M       (16M~63M)
} NAND_MAP;

/* 文件系统控制块 */
typedef struct {
    SYS_TREE *root;                     //树根
    SYS_TREE systree[TREE_FILE_MAX];    //文件树
    NAND_MAP nandmap;                   //nand使用映射表
} FILESYSTEM_CTRL;

/* 文件树字串解析结果链表 */
typedef struct path_list {
    char name[32];              //文件/路径名
    struct path_list *next;     //下个节点
} PATH_LIST;
#endif


/* nandflash容量：256M 
0~15M：存放bin文件
15M~16M：存放文件系统控制块
16M~256M：存放文件
*/

#define NAND_BEGIN      (1024 * 1024 * 15)          //文件系统起始地址
#define NAND_END        (1024 * 1024 * 256)         //文件系统结束地址
#define NAND_TREE_SIZE  (1024 * 1024)               //文件系统控制块分配大小，实际大小为sizeof(FILESYSTEM_CTRL),实际大小必须小于分配大小
#define TREE_FILE_MAX   (1024)                      //文件最大数量

/* 各种block块nand映射数量 */
enum NAND_MAP_NUM {
    NAND_32M_NUM  = 1,
    NAND_16M_NUM  = 2,
    NAND_8M_NUM   = 4,
    NAND_4M_NUM   = 8,
    NAND_1M_NUM   = 32,
    NAND_512K_NUM = 64,
    NAND_128K_NUM = 384,
};

/* 各种block块nand映射基地址 */
enum NAND_MAP_BASE {
    NAND_32M_BASE  = 224 * 1024 * 1024,
    NAND_16M_BASE  = 192 * 1024 * 1024,
    NAND_8M_BASE   = 160 * 1024 * 1024,
    NAND_4M_BASE   = 128 * 1024 * 1024,
    NAND_1M_BASE   = 96  * 1024 * 1024,
    NAND_512K_BASE = 64  * 1024 * 1024,
    NAND_128K_BASE = 16  * 1024 * 1024,
};

/* 各种block块nand映射大小 */
enum NAND_MAP_SIZE {
    NAND_32M_SIZE  = 32  * 1024 * 1024,
    NAND_16M_SIZE  = 16  * 1024 * 1024,
    NAND_8M_SIZE   = 8   * 1024 * 1024,
    NAND_4M_SIZE   = 4   * 1024 * 1024,
    NAND_1M_SIZE   = 1   * 1024 * 1024,
    NAND_512K_SIZE = 512 * 1024,
    NAND_128K_SIZE = 128 * 1024,
};

/* nand使用映射表 */
typedef struct {
    /*                       存储块大小        存储块数量        存储块总大小         存储块区间 */
    int nand_32M;        //      32M     *     1      =      32M       (224M~255M)
    int nand_16M;        //      16M     *     2      =      32M       (192M~223M)
    int nand_8M;         //      8M      *     4      =      32M       (160M~191M)
    int nand_4M;         //      4M      *     8      =      32M       (128M~159M)
    int nand_1M;         //      1M      *     32     =      32M       (96M~127M)
    int nand_512k[2];    //      512k    *     64     =      32M       (64M~95M)
    int nand_128k[12];   //      128k    *     384    =      48M       (16M~63M)
} NAND_MAP;

/* 文件树 */
typedef struct sys_tree {
    int size;                       //文件大小
    int nandaddr;                   //nand存储首地址
    char name[32];                  //文件名
} SYS_TREE;

/* 文件系统控制块 */
typedef struct {
    SYS_TREE systree[TREE_FILE_MAX];    //文件树
    NAND_MAP nandmap;                   //nand使用映射表
} FILESYSTEM_CTRL;

void filesystem_init();
SYS_TREE* filesystem_open_file(char *path);
int filesystem_read_file(SYS_TREE* file, char *buf, int size);


#endif
