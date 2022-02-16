/**
 * @brief sys_context_swap(cur_ctx, new_ctx)
 */
.globl sys_context_swap
.type  sys_context_swap, @function
sys_context_swap:
    leaq (%rsp),%rax
    movq %rax, 104(%rdi)    // rsp寄存器 -> cur_ctx->regs[13]
    movq %rbx, 96(%rdi)     // rbx寄存器 -> cur_ctx->regs[12]
    movq %rcx, 88(%rdi)     // rcx寄存器 -> cur_ctx->regs[11]
    movq %rdx, 80(%rdi)     // rdx寄存器 -> cur_ctx->regs[10]
    movq 0(%rax), %rax
    movq %rax, 72(%rdi)     // rsp[0]   -> cur_ctx->regs[9]
    movq %rsi, 64(%rdi)     // rsi寄存器 -> cur_ctx->regs[8]
    movq %rdi, 56(%rdi)     // rdi寄存器 -> cur_ctx->regs[7]
    movq %rbp, 48(%rdi)     // rbp寄存器 -> cur_ctx->regs[6]
    movq %r8, 40(%rdi)      // r8寄存器  -> cur_ctx->regs[5]
    movq %r9, 32(%rdi)      // r9寄存器  -> cur_ctx->regs[4]
    movq %r12, 24(%rdi)     // r12寄存器 -> cur_ctx->regs[3]
    movq %r13, 16(%rdi)     // r13寄存器 -> cur_ctx->regs[2]
    movq %r14, 8(%rdi)      // r14寄存器 -> cur_ctx->regs[1]
    movq %r15, (%rdi)       // r15寄存器 -> cur_ctx->regs[0]
    xorq %rax, %rax         // 重置rax寄存器

    movq (%rsi), %r15       // new_ctx->regs[0]  -> r15寄存器
    movq 8(%rsi), %r14      // new_ctx->regs[1]  -> r14寄存器
    movq 16(%rsi), %r13     // new_ctx->regs[2]  -> r13寄存器
    movq 24(%rsi), %r12     // new_ctx->regs[3]  -> r12寄存器
    movq 32(%rsi), %r9      // new_ctx->regs[4]  -> r9寄存器
    movq 40(%rsi), %r8      // new_ctx->regs[5]  -> r8寄存器
    movq 48(%rsi), %rbp     // new_ctx->regs[6]  -> rbp栈底寄存器
    movq 56(%rsi), %rdi     // new_ctx->regs[7]  -> 参数2寄存器
    movq 80(%rsi), %rdx     // new_ctx->regs[10] -> rdx寄存器
    movq 88(%rsi), %rcx     // new_ctx->regs[11] -> rcx寄存器
    movq 96(%rsi), %rbx     // new_ctx->regs[12] -> rbx寄存器
    movq 104(%rsi), %rsp    // new_ctx->regs[13] -> rsp栈顶寄存器
    leaq 8(%rsp), %rsp      // new_ctx->ss_sp    -> rsp寄存器 (新协程的栈地址赋值到rsp寄存器)
    pushq 72(%rsi)          // new_ctx->regs[9]  -> rsp[0] (函数返回地址, )

    movq 64(%rsi), %rsi     // new_ctx->regs[8](协程控制块stCoRoutine_t的地址) -> 参数1寄存器
    ret