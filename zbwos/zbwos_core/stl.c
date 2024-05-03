#include "stl.h"



/* 前导0计算(计算一个32位无符号数前有几个0)
    例：0x0001结果为31
        0x5000结果为17
*/
unsigned char LeadZeros(unsigned int val) {
    static const unsigned char  LeadZerosTbl[256] = {
        8u, 7u, 6u, 6u, 5u, 5u, 5u, 5u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
        3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u,
        2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u,
        2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u,
        1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
        1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
        1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
        1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
        
    if (val > 0x0000FFFFu) {
        if (val > 0x00FFFFFFu) {
            return LeadZerosTbl[val >> 24];
        } else {
            return LeadZerosTbl[val >> 16] + 8;
        } 
    } else {
        if (val > 0x000000FFu) {
            return LeadZerosTbl[val >> 8] + 16;
        } else {
            return LeadZerosTbl[val] + 24;
        } 
    }
}

/* 字符串转整数（c库atoi和atol均有点问题，故重新实现） */
int _atoi(char *s) {
    int ret = 0;
    if (!s) return 0;
    do {
        ret = ret * 10 + *s - '0';
    } while (++s && *s);
    return ret;
}

/* 两数交换 */
void swap_xy(int *px, int *py) {
    *px ^= *py ^= *px ^= *py;
    return;
}

