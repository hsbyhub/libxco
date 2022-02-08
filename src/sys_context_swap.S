/**
 * @brief sys_context_swap(cur_ctx, new_ctx)
 */
.globl sys_context_swap
.type  sys_context_swap, @function
sys_context_swap:
    leaq (%rsp),%rax
    movq %rax, 104(%rdi)    // rsp¼Ä´æÆ÷ -> cur_ctx->regs[13]
    movq %rbx, 96(%rdi)     // rbx¼Ä´æÆ÷ -> cur_ctx->regs[12]
    movq %rcx, 88(%rdi)     // rcx¼Ä´æÆ÷ -> cur_ctx->regs[11]
    movq %rdx, 80(%rdi)     // rdx¼Ä´æÆ÷ -> cur_ctx->regs[10]
    movq 0(%rax), %rax
    movq %rax, 72(%rdi)     // rsp[0]   -> cur_ctx->regs[9]
    movq %rsi, 64(%rdi)     // rsi¼Ä´æÆ÷ -> cur_ctx->regs[8]
    movq %rdi, 56(%rdi)     // rdi¼Ä´æÆ÷ -> cur_ctx->regs[7]
    movq %rbp, 48(%rdi)     // rbp¼Ä´æÆ÷ -> cur_ctx->regs[6]
    movq %r8, 40(%rdi)      // r8¼Ä´æÆ÷  -> cur_ctx->regs[5]
    movq %r9, 32(%rdi)      // r9¼Ä´æÆ÷  -> cur_ctx->regs[4]
    movq %r12, 24(%rdi)     // r12¼Ä´æÆ÷ -> cur_ctx->regs[3]
    movq %r13, 16(%rdi)     // r13¼Ä´æÆ÷ -> cur_ctx->regs[2]
    movq %r14, 8(%rdi)      // r14¼Ä´æÆ÷ -> cur_ctx->regs[1]
    movq %r15, (%rdi)       // r15¼Ä´æÆ÷ -> cur_ctx->regs[0]
    xorq %rax, %rax         // ÖØÖÃrax¼Ä´æÆ÷

    movq (%rsi), %r15       // new_ctx->regs[0]  -> r15¼Ä´æÆ÷
    movq 8(%rsi), %r14      // new_ctx->regs[1]  -> r14¼Ä´æÆ÷
    movq 16(%rsi), %r13     // new_ctx->regs[2]  -> r13¼Ä´æÆ÷
    movq 24(%rsi), %r12     // new_ctx->regs[3]  -> r12¼Ä´æÆ÷
    movq 32(%rsi), %r9      // new_ctx->regs[4]  -> r9¼Ä´æÆ÷
    movq 40(%rsi), %r8      // new_ctx->regs[5]  -> r8¼Ä´æÆ÷
    movq 48(%rsi), %rbp     // new_ctx->regs[6]  -> rbpÕ»µ×¼Ä´æÆ÷
    movq 56(%rsi), %rdi     // new_ctx->regs[7]  -> ²ÎÊı2¼Ä´æÆ÷
    movq 80(%rsi), %rdx     // new_ctx->regs[10] -> rdx¼Ä´æÆ÷
    movq 88(%rsi), %rcx     // new_ctx->regs[11] -> rcx¼Ä´æÆ÷
    movq 96(%rsi), %rbx     // new_ctx->regs[12] -> rbx¼Ä´æÆ÷
    movq 104(%rsi), %rsp    // new_ctx->regs[13] -> rspÕ»¶¥¼Ä´æÆ÷
    leaq 8(%rsp), %rsp      // new_ctx->ss_sp    -> rsp¼Ä´æÆ÷ (ĞÂĞ­³ÌµÄÕ»µØÖ·¸³Öµµ½rsp¼Ä´æÆ÷)
    pushq 72(%rsi)          // new_ctx->regs[9]  -> rsp[0] (º¯Êı·µ»ØµØÖ·, )

    movq 64(%rsi), %rsi     // new_ctx->regs[8](Ğ­³Ì¿ØÖÆ¿éstCoRoutine_tµÄµØÖ·) -> ²ÎÊı1¼Ä´æÆ÷
    ret