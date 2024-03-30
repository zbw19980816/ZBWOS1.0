#include "tjpgd.h"
#include "heap.h"
#include "stl.h"
#include "filesystem.h"

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

    /* 输出进度 */
    if (rect->left == 0)
    {
        printf("%d\r\n", (rect->top << jd->scale) * 100UL / jd->height);
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
int jpeg_decode_from_buf(char *jpeg_buf, int jpeg_buf_size, char **rgb_buf) {
    int ret = 0;
    JDEC jdec;          //解码对象
    char *work;         //算法内存
    JPEG_PROC jpeg_proc;  //jpeg处理信息

    work = New(3100);
    if (!work) {
        printf("new work fail\r\n");
        return -1;
    }
    
    jpeg_proc.jpegbuf = jpeg_buf;
    jpeg_proc.seek = 0;
    
    ret = jd_prepare(&jdec, in_func, work, 3100, &jpeg_proc);
    if (ret != JDR_OK) {
        printf("Failed to prepare: rc = %d\r\n", ret);
        Delete(work);
        return -1;
    }

    /* 准备好解码，图像信息有效 */
    printf("Image dimensions: %u * %u. %u bytes used.\r\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

    jpeg_proc.width = jdec.width;
    jpeg_proc.rgbbuf = New(3 * jdec.width * jdec.height);
    if (!jpeg_proc.rgbbuf) {
        printf("new jpeg_proc.rgbbuf fail\r\n");
        Delete(work);
        return -1;
    }

    ret = jd_decomp(&jdec, out_func, JD_SCALE);   /* 开始解码(1/1缩放) */
    if (ret != JDR_OK) {
        printf("Failed to decompress: rc=%d\r\n", ret);
        Delete(work);
        Delete(jpeg_proc.rgbbuf);
        return -1;
    }
    
    Delete(work);
    *rgb_buf = jpeg_proc.rgbbuf;
    return 3 * jdec.width * jdec.height;
}

/* jpeg解码（文件流）
rgb_buf由外部释放
返回rgb_buf长度 */
int jpeg_decode_from_file(char *jpeg_name, char **rgb_buf) {
    char *jpeg_buf = NULL;
    int ret = 0;
    SYS_TREE *p = filesystem_open_file(jpeg_name);

    if (NULL == p) {
        printf("filesystem_open_file %s fail\r\n", jpeg_name);
        return -1;
    }

    jpeg_buf = New(p->size);
    if (!jpeg_buf) {
        printf("new jpeg_buf fail\r\n");
        return -1;
    }
    
    ret = filesystem_read_file(p, jpeg_buf, p->size);
    if (ret <= 0) {
        printf("filesystem_read_file fail, ret[%d]\r\n", ret);
        Delete(jpeg_buf);
        return -1;
    }

    ret = jpeg_decode_from_buf(jpeg_buf, p->size, rgb_buf);
    if (ret <= 0) {
        printf("jpeg_decode_from_buf fail, ret[%d]\r\n", ret);
    }
    
    Delete(jpeg_buf);
    return ret;
}

