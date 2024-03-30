/* JPEG解码单元测试 */

#include "../tjpgd.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int seek;       //偏移
    int width;      //jpeg宽度
    char *jpegbuf;  //jpeg缓存地址
    char *rgbbuf;   //解码结果存放缓存
} JPEG_PROC;

/* jpeg数据输入回调 */
uint16_t in_func(JDEC *jd, unsigned char *buff, uint16_t nbyte) {
    JPEG_PROC *jpeg_proc = (JPEG_PROC *)jd->device;
    if (buff) {
        /* 从jpegbuf读取nbyte字节 */
        memcpy(buff, jpeg_proc->jpegbuf + jpeg_proc->seek, nbyte);
    }

    /* 更新偏移 */
    jpeg_proc->seek += nbyte;
    return nbyte;
}

/* jpeg数据输出回调 */
uint16_t out_func (JDEC *jd, void *bitmap, JRECT *rect)
{
    JPEG_PROC *dev = (JPEG_PROC *)jd->device;
    unsigned char *src, *dst;
    unsigned int y, bws, bwd;

    static int output_cnt = 0;
    printf("output count = %d\r\n", output_cnt++);

    /* 输出进度 */
    if (rect->left == 0)
    {
        printf("%lu%%\r\n", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* 拷贝解码的RGB矩形范围到帧缓冲区(RGB888配置) */
    src = (unsigned char *)bitmap;
    
    /* 目标矩形的左上 */
    dst = dev->rgbbuf + 3 * (rect->top * dev->width / (1 << JD_SCALE) + rect->left);
    bws = 3 * (rect->right - rect->left + 1);           /* 源矩形的宽度[字节] */
    bwd = 3 * dev->width / (1 << JD_SCALE);             /* 帧缓冲区宽度[字节] */
    for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws);   /* 拷贝一行 */
        src += bws;
        dst += bwd;  /* 定位下一行 */
    }

    return 1;    /* 继续解码 */
}

/* jpeg解码（数据流）
rgb_buf由外部释放
返回rgb_buf长度 */
int jpeg_decode_from_buf_untest(char *jpeg_buf, int jpeg_buf_size, char **rgb_buf) {
    int ret = 0;
    JDEC jdec;          //解码对象
    char *work;         //算法内存
    JPEG_PROC jpeg_proc;  //jpeg处理信息

    work = malloc(3100);
    if (!work) {
        printf("new work fail");
        return -1;
    }
    
    jpeg_proc.jpegbuf = jpeg_buf;
    jpeg_proc.seek = 0;
    
    ret = jd_prepare(&jdec, in_func, work, 3100, &jpeg_proc);
    if (ret != JDR_OK) {
        printf("Failed to prepare: rc = %d\n", ret);
        free(work);
        return -1;
    }

    /* 准备好解码，图像信息有效 */
    printf("Image dimensions: %u * %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

    jpeg_proc.width = jdec.width;
    jpeg_proc.rgbbuf = malloc(3 * jdec.width * jdec.height);
    if (!jpeg_proc.rgbbuf) {
        printf("new jpeg_proc.rgbbuf fail");
        free(work);
        return -1;
    }

    ret = jd_decomp(&jdec, out_func, JD_SCALE);   /* 开始解码(1/1缩放) */
    if (ret != JDR_OK) {
        printf("Failed to decompress: rc=%d\n", ret);
        free(work);
        free(jpeg_proc.rgbbuf);
        return -1;
    }
    
    free(work);
    *rgb_buf = jpeg_proc.rgbbuf;
    return 3 * jdec.width * jdec.height;
}

//入参1：jpeg路径  
//入参2：生成rgb文件名
//rgb格式：RGB888, JD_FORMAT       0   /* Output pixel format 0:RGB888 (3 BYTE/pix), 1:RGB565 (1 WORD/pix) */
//缩放：1:1
int main(int argc, char *argv[]) 
{
    FILE *fp;
    struct stat st;
    char *jpeg_buf = NULL;
    char *rgb_buf = NULL;
    
    if (argc < 3) {
        printf("please input jpeg name[in] and rgb name[out]!\n");
        return -1;
    }

    /* 打开一个JPEG文件 */
    fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("fopen %s fail\n", argv[1]);
        return -1;
    }
    
    int ret = stat(argv[1], &st);
    if (-1 == ret)  {
        printf("stat %s fail\n", argv[1]);
        return -1;
    }
    
    printf("%s size = %d\n", argv[1], st.st_size);

    jpeg_buf = malloc(st.st_size);
    if (NULL == jpeg_buf) {
        printf("malloc fail\n");
        return -1;
    }
    
    fread(jpeg_buf, 1, st.st_size, fp);
    fclose(fp);
    ret = jpeg_decode_from_buf_untest(jpeg_buf, st.st_size, &rgb_buf);
    if (-1 == ret)  {
        free(jpeg_buf);
        printf("jpeg_decode_from_buf_untest fail, ret = %d\n", ret);
        return -1;
    }

    fp = fopen(argv[2], "w");
    fwrite(rgb_buf, ret, 1, fp);
    fclose(fp);
    
    return 0;
}

