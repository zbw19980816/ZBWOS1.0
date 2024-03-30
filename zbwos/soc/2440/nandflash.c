#include "nandflash.h"
#include "s3c2440_soc.h"


/* 初始化nand flash */
void nand_init(void) {
    /* 使能nand flash时序 */
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /* 使能nand flash 控制器，初始化ECC，禁止片选 */
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);

    return;
}

/* 使能片选 */
void nand_select(void) {
    NFCONT &= ~(1 << 1);
    return;
}

/* 禁止片选 */
void nand_deselect(void) {
    NFCONT |= (1 << 1);
    return;
}

/* 发命令 */
void nand_cmd(unsigned char cmd) {
    volatile int i;
    NFCMD = cmd;
    for (i = 0; i < 10; ++i);   //保证数据信号的稳定
    return;
}

/* 发地址 */
void nand_addr_byte(unsigned char addr) {
    volatile int i;
    NFADDR = addr;
    for (i = 0; i < 10; ++i);   //保证数据信号的稳定
    return;
}

/* 读取数据 */
unsigned char nand_data(void){
    return NFDATA;
}

/* 烧写数据 */
void nand_w_data(unsigned char data) {
    NFDATA = data;
}

/* 读取nand flash芯片id（获取到了说明nand flash初始化成功并且可以正常工作） */
void nand_chip_id(void) {
    unsigned char buf[5] = {0};
    nand_select();
    nand_cmd(0x90);
    nand_addr_byte(0x00);
    buf[0] = nand_data();
    buf[1] = nand_data();
    buf[2] = nand_data();
    buf[3] = nand_data();
    buf[4] = nand_data();
    nand_deselect();
    pri_byte("maker id = ", buf[0]);
    pri_byte("device id = ", buf[1]);
    pri_byte("3 byte = ", buf[2]);
    pri_byte("4 byte id = ", buf[3]);
    pri("page size = ", (1 << (buf[3] & 0x03)));  //页大小2kb
    pri("block size = ", (64 << ((buf[3] >> 4) & 0x03)));  //块大小128kb
    pri_byte("5 byte id = ", buf[4]);
    
    return;
}

/* 查看nand flash是在忙还是已就绪 */
void wait_ready() {
    while (!(NFSTAT & 1));
}

/* 读nanf flash:从addr读len放到buf */
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len) {
    int i = 0;
    int page = addr / 2048;       //哪页(行)
    int col = addr & (2048 - 1);   //哪列
    
    nand_select();
    
    /* 每次最多读1page(2KB)数据，假如超过1page需要循环读取 */
    while (i < len) {
        //发00读命令
        nand_cmd(0x00);
        
        //发地址
        //col addr
        nand_addr_byte(col & 0xff);
        nand_addr_byte((col >> 8) & 0xff);
        //row/page addr
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page >> 8) & 0xff);
        nand_addr_byte((page >> 16) & 0xff);
        
        //发30命令
        nand_cmd(0x30); 
        
        //等待就绪
        wait_ready();
        
        //读数据
        for (; (col < 2048) && (i < len); ++col) {
            buf[i++] = nand_data();
        }
        
        col = 0;
        ++page;
    }
    
    nand_deselect();
    return;
}

/* nand flash擦除:从addr开始擦除len;nand擦除是以块为单位的,1个块为128k */
void nand_erase(unsigned int addr, unsigned int len) {
    int page = addr / 2048;

    if ((addr & 0x1FFFF) || (len & 0x1FFFF)) {
        /* addr或len不是128k(块大小)整数倍,无法擦除 */
        puts("addr or len is not block align\r\n");
        return -1;
    }
    
    nand_select();
    while (len != 0) {
        /* 计算页行号 */
        page = addr / 2048;
        
        //发60命令
        nand_cmd(0x60);

        /* 发地址row/page addr */
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page >> 8) & 0xff);
        nand_addr_byte((page >> 16) & 0xff);

        //发D0命令
        nand_cmd(0xD0);

        //等待完成
        wait_ready();

        len -= 0x20000;
        addr += 0x20000;
    }
    
    nand_deselect();
    return 0;
}

/* 将buf上len长度的数据写到addr上 */
void nand_write(unsigned int addr, unsigned char *buf, unsigned int len) {
    int i = 0;
    int page = addr / 2048;       //哪页(行)
    int col = addr & (2048 - 1);   //哪列

    nand_select();
    
   // printf("len = %d\r\n", len);
    while (i < len) {
       // printf("i = %d\r\n", i);
        //发80命令
        nand_cmd(0x80);

        //发地址
        //col addr
        nand_addr_byte(col & 0xff);
        nand_addr_byte((col >> 8) & 0xff);
        //row/page addr
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page >> 8) & 0xff);
        nand_addr_byte((page >> 16) & 0xff);
        
        /* 写数据 */
        while ((col++ < 2048) && (i < len)) {
            nand_w_data(buf[i++]);
        }

        //发10命令
        nand_cmd(0x10);

        //等待烧写完成
        wait_ready();

        col = 0;
        ++page;
    }
    
    nand_deselect();
    return;
}


/* 读/写/擦除测试 */
void nand_test() {
    puts("nand_test ...444 \r\n");
    return ;
    int i = 0;
    nand_init();
    puts("nand_test ...555 \r\n");

    /* 读取id测试 */
    nand_chip_id();

    /* 读测试 */
    unsigned char buf[16] = {0};
    nand_read(0, buf, sizeof(buf));
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("1 nand_read buf: ", buf[i]);  //.bin的前16个byte
    }
    puts("zbwos ...444 \r\n");

    /* 擦除测试 */
    nand_read(1024 * 128, buf, sizeof(buf));  //先读出128k处的数据
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("2 nand_read buf: ", buf[i]);  //128k处的数据(乱数据)
    }
    
    nand_erase(1024 * 1024 * 16, 1024 * 128);  //擦除128k~256k

    nand_read(1024 * 1024 * 16, buf, sizeof(buf));  //重新读出128k处的数据
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("3 nand_read buf: ", buf[i]);  //128k处的数据(全是1)
    }
    
    /* 写测试 */
    unsigned char *wbuf = "hello nand";
    nand_write(1024 * 1024 * 16, wbuf, 10);   //往128k处写入hello nand

    nand_read(1024 * 1024 * 16, buf, sizeof(buf));  //重新读出128k处的数据
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("4 nand_read buf: ", buf[i]);  //128k处的数据(hello nand)
    }

    unsigned char *wbuf2 = "0123456789";
    nand_write(1024 * 128, wbuf2, 10);   //往128k处写入0123456789

    nand_read(1024 * 128, buf, sizeof(buf));  //重新读出128k处的数据
    for (i = 0; i < sizeof(buf); ++i) {
        //128k处的数据(乱数据) nand写只支持将1转成0，例如128k处本来写了'h'(0x68),后面写入'0'(0x30)
        //0x68 = 0110 1000
        //0x30 = 0011 0000
        //结果 = 0010 0000('h'原来已经将第4位置0，所以'0'写入就不对)
        pri_byte("5 nand_read buf: ", buf[i]);  
    }

    nand_erase(1024 * 128, 1024 * 128);  //擦除128k~256k

    nand_read(1024 * 128, buf, sizeof(buf));  //重新读出128k处的数据
    for (i = 0; i < sizeof(buf); ++i) {
        pri_byte("6 nand_read buf: ", buf[i]);  //128k处的数据
    }
    puts("zbwos ...555 \r\n");
while(1);
    return;
}

