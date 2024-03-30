
#ifndef __JPEG_PROC_H_
#define __JPEG_PROC_H_

int jpeg_decode_from_file(char *jpeg_name, char **rgb_buf);
int jpeg_decode_from_buf(char *jpeg_buf, int jpeg_buf_size, char **rgb_buf);


#endif 
