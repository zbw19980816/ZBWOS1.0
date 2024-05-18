
#ifndef __JPEG_PROC_H_
#define __JPEG_PROC_H_

//BMP头部
typedef struct {
    //位图文件头（14byte）
    char bmName[2];         //"BM"，即0x424D
    int size;               //整个BMP文件的大小
    int res;                //保留
    int seek;               //文件起始位置到图像像素数据的字节偏移量,一般为54

    //位图信息头（40byte）
    int biSize;             //位图信息头所需要的字节数,一般为40
    int biWidth;            //图像的宽度
    int biHeight;           //图像的高度
    short biPlanes;         //为目标设备说明位面数，其值总设置为1
    short biBitCount;       //一个像素点占几位（以比特位/像素位单位），其值可为1,4,8,16,24或32
    int biCompression;      //图像数据的压缩类型
    int biSizeImage;        //图像的大小
    int biXPelsPerMeter;    //水平分辨率
    int biYPelsPerMeter;    //垂直分辨率
    int biClrUsed;          //位图实际使用的调色板索引数，0：使用所有的调色板索引
    int biClrImportant;     //对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要
} BITMAP_HEAD_S;

//BMP
typedef struct {
    BITMAP_HEAD_S head;   //头
    char* data;             //数据
} BITMAP_S;

int jpeg_decode_from_file(char *jpeg_name, BITMAP_S *bmp);
int jpeg_decode_from_buf(char *jpeg_buf, int jpeg_buf_size, BITMAP_S *bmp);


#endif 
