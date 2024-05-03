/* uboot写死启动地址: 0x30007FC0 */

.text
.global _start
.global StartTask_asm
.global _reboot

_start:
/* 0、设置中断向量表 */
	b reset          /* vector 0 : reset */
	ldr pc, und_addr /* vector 4 : und */
	ldr pc, swi_addr /* vector 8 : swi */
	b halt			 /* vector 0x0c : prefetch aboot */
	b halt			 /* vector 0x10 : data abort */
	b halt			 /* vector 0x14 : reserved */
	ldr pc,   irq_addr /* vector 0x18 : irq */
	b halt			 /* vector 0x1c : fiq */

und_addr:
	.word do_und

swi_addr:
	.word do_swi

irq_addr:
    /*.word do_irq_2440*/
	.word do_irq_task

do_und:
	/* 执行到这里之前:
	 * 1. lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/* sp_und未设置, 先设置它 */
	ldr sp, =0x34000000

	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  
	
	/* 保存现场 */
	/* 处理und异常 */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
und_string:
	.string "undefined instruction exception"

.align 4

do_swi:
	/* 执行到这里之前:
	 * 1. lr_svc保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_svc保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10011, 进入到svc模式
	 * 4. 跳到0x08的地方执行程序 
	 */

	/* sp_svc未设置, 先设置它 */
	ldr sp, =0x33e00000

	/* 保存现场 */
	/* 在swi异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  

	mov r4, lr
	
	/* 处理swi异常 */
	mrs r0, cpsr
	ldr r1, =swi_string
	bl printException

	sub r0, r4, #4
	bl printSWIVal
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
swi_string:
	.string "swi exception"

.align 4
	
do_irq_2440:
	/* 执行到这里之前:
	 * 1. lr_irq保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_irq保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10010, 进入到irq模式
	 * 4. 跳到0x18的地方执行程序 
	 */

	/* sp_irq未设置, 先设置它 */
	ldr sp, =0x33d00000

	/* 保存现场 */
	/* 在irq异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr-4是异常处理完后的返回地址, 也要保存 */
	sub lr, lr, #4
	stmdb sp!, {r0-r12, lr}  
	
	/* 处理irq异常 */
	/*mov lr, r0*/
	bl handle_irq_c
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr_irq的值恢复到cpsr里 */

reset:
	ldr sp, =0x33f00000
	bl sdram_init
	bl clean_bss


    /***/
	ldr r0, =0x4c000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/* CLKDIVN（0x4c000014）0x5 */
	ldr r0, =0x4c000014
	ldr r1, =0x5
	str r1, [r0]

	/* 将cpu设置为异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0

	/* 设置MPLLCON（0x4c000004）=(92<<12)|(1<<4)|(1<<0) */
	ldr r0, =0x4c000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]

    mrs r0, cpsr         /* 读出cpsr */
	bic r0, r0, #0xf     /* 修改M4-M0为0b10000, 进入usr模式 */
	bic r0, r0, #(1<<7)  /* 清除I位, 使能中断 */
	msr cpsr, r0
	
	ldr sp, =0x33f80000  /* uboot start addr */
	ldr pc, =main
    /*ldr pc, =_Z4mainv*/
/* 1、关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]

/* 2、设置时钟(400M) */
	/* 设置MPLL，FCLK:HCLK:PCLK = 400M:100M:50M */
	/* FCLK:CPU时钟 HCLK:高速外设时钟（LCD、中断等） PCLK低速外设时钟（GPIO、IIC等） */
	/* LOCKTIME（0x4c000000）= 0xFFFFFFFF */
	ldr r0, =0x4c000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/* CLKDIVN（0x4c000014）0x5 */
	ldr r0, =0x4c000014
	ldr r1, =0x5
	str r1, [r0]

	/* 将cpu设置为异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0

	/* 设置MPLLCON（0x4c000004）=(92<<12)|(1<<4)|(1<<0) */
	ldr r0, =0x4c000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]

	/* 一旦设置PLL，就会锁定lock time直到cpu工作稳定 */
	/* 然后cpu工作与新的频率FCLK(400M) */
	
/* 3、设置内存：sp栈 */
	/* 分辨是nor/nand启动：
		写0到0地址，再读出来
		如果得到0，说明是0地址已经被改变，只有RAM内数据可以被改变
		得到0就是nand启动（因为nand启动会把代码加载到片内RAM）
		得到非0就是nor启动
	*/
	ldr r0, =0
	ldr r1, [r0]  				/* 将0地址内容先存到r1，可用于后续恢复 */
	str r0, [r0]  				/* 在0地址写入0 */
	ldr r2, [r0]  				/* 重新读取0地址内容到r2 */
	cmp r2, r0    				/* 将第二次读到的内容和0比较，相等就是nand启动 */
	ldr sp, =0x34000000  	    /* 设置为nor启动，后续判断cmp结果是相等后可能会修改sp值 */
	moveq sp, #4096 			/* cmp r2, r0相等才会执行这句 */
	str r1, [r0]				/* 恢复0地址的值 */

/* 4、初始化sdram（这步一定要放在重定位data段之前（重定位过程需要拷贝数据至sdram，所以要先初始化sdram）） */
/*	bl sdram_init*/

/* 5、重定位text、rodata、data段 */
/*	bl copy2sdram*/
	
/* 6、清除bss段 */
/*	bl clean_bss*/

	/* 复位之后, cpu处于svc模式
	 * 现在, 切换到usr模式
	 */
	mrs r0, cpsr         /* 读出cpsr */
	bic r0, r0, #0xf     /* 修改M4-M0为0b10000, 进入usr模式 */
	bic r0, r0, #(1<<7)  /* 清除I位, 使能中断 */
	msr cpsr, r0

	/* 设置 sp_usr???????????不知道干嘛用，先屏蔽(07/23:不加启动时会有undefined instruction exception异常) */
	ldr sp, =0x33f00000
	
	/*ldr pc, =main*/

halt:
	b halt

/* 从任务栈里恢复r4~r11 */
StartTask_asm:
	/* 将r0为首地址处内存连续读8*4个byte，传给r4~r11，同时r0+=4*8 */
	ldmia r0!, { r4 - r11 }

	/* 更新sp为r0（r0此时指向栈顶）msr sp, r0 */
	mov sp, r0
	
/* 触发硬件中断返回，恢复其他寄存器(r1中存了中断返回的地址(特殊的lr)) */
	bx r1

/* 重启设备 */
_reboot:
    .word 0xdeadc0de  /* 未定义指令 */
	swi 0x123  /* 执行此命令, 触发SWI异常, 通过uboot重启 */
	/* 1、开启看门狗 */
	/*ldr r0, =0x53000000
	ldr r1, =0x8011
	str r1, [r0]*/