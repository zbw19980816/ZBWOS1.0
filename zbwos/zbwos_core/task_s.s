/* 依赖：uboot IRQ_STACK_START = 0x34000000 */


.text
.align 4
.global do_irq_task
.global Enter_Critical_S
.global Exit_Critical_S

do_irq_task:

/* 保存现场 */
save_reg:
	stmia sp, {sp}^				/* 1、获取用户下sp值 */
	ldr sp, [sp]				/* 2、将异常模式的sp值设置成用户的sp值 */

	sub sp, sp, #8
	/*stmdb sp!, {r0-r14}^*/		/* 3、异常下存储r0-r14寄存器值 */
    /* uboot */
    stmdb sp!, {r13-r14}^      /* 用户模式下sp(r13)和lr(r14) */
    ldr lr, =0x34000000   /* IRQ_STACK_START~IRQ_STACK_START-13*4  ===> lr r12~r0*/

    /* r12 */
    sub lr, lr, #8
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r11 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r10 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r9 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r8 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r7 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r6 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r5 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r4 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r3 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]
    
    /* r2 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r1 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]

    /* r0 */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]
    
	add sp, sp, #68
	mrs r0, spsr
	stmdb sp!, {r0}				/* 4、存储用户模式cpsr */
    ldr lr, =0x34000000   /* IRQ_STACK_START~IRQ_STACK_START-13*4  ===> lr r12~r0*/

    /* 用户lr */
    sub lr, lr, #4
    sub sp, sp, #4
    ldr r0, [lr]
    str r0, [sp]
    
/* 切换现场 */
	sub sp, sp, #60
	mov r0, sp
	/*bl handle_irq_task			/* 1、清中断(需查看：handle_irq_task反汇编中不可以使用r0) */
	bl saveandupdate_curstack	/* 2、将上个任务栈顶更新成用户模式sp-17 */
	
/* 恢复现场 */	
start_task:
	mov sp, r0				/* sp存了栈顶地址 */
	ldmia sp!, {r0-r14}^ 	/* 恢复r0-r14 */
	
	add lr, sp, #4      	/* lr存入用户模式cpsr值地址 */
	ldr lr, [lr] 			/* lr存入用户模式cpsr值 */
	msr spsr, lr        	/* 更新用户模式cpsr */
	/*sub sp, sp, #4          /* 1126新增 */
	ldmia sp, {pc}^     	/* 恢复PC，同时恢复CPSR（进入User模式），执行新任务    */

/* 进入临界段 */
Enter_Critical_S:
	mrs r0, cpsr         /* 读出cpsr */
	orr r0, r0, #(1<<7)  /* 设置I位, 失能中断 */
	msr cpsr, r0
	
/* 退出临界段 */
Exit_Critical_S:
	mrs r0, cpsr         /* 读出cpsr */
	bic r0, r0, #(1<<7)  /* 清除I位, 使能中断 */
	msr cpsr, r0

