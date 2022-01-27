/**
 * @brief sys_context_swap(cur_ctx, new_ctx)
 */
.globl sys_context_swap
sys_context_swap:
leaq (%rsp),%rax
movq %rax, 104(%rdi)    // rsp�Ĵ��� -> new_ctx->regs[13]
movq %rbx, 96(%rdi)     // rbx�Ĵ��� -> new_ctx->regs[12]
movq %rcx, 88(%rdi)     // rcx�Ĵ��� -> new_ctx->regs[11]
movq %rdx, 80(%rdi)     // rdx�Ĵ��� -> new_ctx->regs[10]
movq 0(%rax), %rax
movq %rax, 72(%rdi)     // rsp[0]   -> new_ctx->regs[9]
movq %rsi, 64(%rdi)     // rsi�Ĵ��� -> new_ctx->regs[8]
movq %rdi, 56(%rdi)     // rdi�Ĵ��� -> new_ctx->regs[7]
movq %rbp, 48(%rdi)     // rbp�Ĵ��� -> new_ctx->regs[6]
movq %r8, 40(%rdi)      // r8�Ĵ���  -> new_ctx->regs[5]
movq %r9, 32(%rdi)      // r9�Ĵ���  -> new_ctx->regs[4]
movq %r12, 24(%rdi)     // r12�Ĵ��� -> new_ctx->regs[3]
movq %r13, 16(%rdi)     // r13�Ĵ��� -> new_ctx->regs[2]
movq %r14, 8(%rdi)      // r14�Ĵ��� -> new_ctx->regs[1]
movq %r15, (%rdi)       // r15�Ĵ��� -> new_ctx->regs[0]
xorq %rax, %rax         // ����rax�Ĵ���

movq (%rsi), %r15       // new_ctx->regs[0]  -> r15�Ĵ���
movq 8(%rsi), %r14      // new_ctx->regs[1]  -> r14�Ĵ���
movq 16(%rsi), %r13     // new_ctx->regs[2]  -> r13�Ĵ���
movq 24(%rsi), %r12     // new_ctx->regs[3]  -> r12�Ĵ���
movq 32(%rsi), %r9      // new_ctx->regs[4]  -> r9�Ĵ���
movq 40(%rsi), %r8      // new_ctx->regs[5]  -> r8�Ĵ���
movq 48(%rsi), %rbp     // new_ctx->regs[6]  -> rbpջ�׼Ĵ���
movq 56(%rsi), %rdi     // new_ctx->regs[7]  -> ����2�Ĵ���
movq 80(%rsi), %rdx     // new_ctx->regs[10] -> rdx�Ĵ���
movq 88(%rsi), %rcx     // new_ctx->regs[11] -> rcx�Ĵ���
movq 96(%rsi), %rbx     // new_ctx->regs[12] -> rbx�Ĵ���
movq 104(%rsi), %rsp    // new_ctx->regs[13] -> rspջ���Ĵ���
leaq 8(%rsp), %rsp      // new_ctx->ss_sp    -> rsp�Ĵ��� (��Э�̵�ջ��ַ��ֵ��rsp�Ĵ���)
pushq 72(%rsi)          // new_ctx->regs[9]  -> rsp[0] (�������ص�ַ, )

movq 64(%rsi), %rsi     // new_ctx->regs[8](Э�̿��ƿ�stCoRoutine_t�ĵ�ַ) -> ����1�Ĵ���
ret