#ifndef __STL_H_
#define __STL_H_

#define NULL (void *)0
#define ALIGN(SIZE, boundary) (((SIZE) + (boundary) - 1) & ~((boundary) - 1))  //内存对齐

unsigned char LeadZeros(unsigned int val);
void swap_xy(int *px, int *py);


#endif
