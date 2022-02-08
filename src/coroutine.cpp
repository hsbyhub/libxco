/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�coroutine.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/1/27
 *================================================================*/
#include "coroutine.h"
#include <cstring>
#include <assert.h>

extern "C" {
void sys_context_swap(xco::Coroutine::SysContext* cur_ctx, xco::Coroutine::SysContext* new_ctx) asm("sys_context_swap");
}

XCO_NAMESPAVE_START
/**
 * @brief Э�̻�����Ϣ
 */
const int   g_co_call_stack_max                     = 128;  // Э�̵���ջ��С
Coroutine*  g_co_call_stack[g_co_call_stack_max]    = {0};  // Э�̵���ջ
int         g_co_call_stack_top                     = -1;   // Э�̵���ջջ��
Coroutine*  g_pending_co                            = nullptr;
Coroutine*  g_occupy_co                             = nullptr;

/**
 * @brief Э����ջ
 */
Coroutine* PullCoroutine();

/**
 * @brief Э�̳�ջ
 */
void PushCoroutine(Coroutine* co);

/**
 * @brief ��ȡ��ǰЭ��
 */
Coroutine* GetCurrentCoroutine();

void PushCoroutine(Coroutine* co) {
    assert(g_co_call_stack_top < g_co_call_stack_max - 1);
    auto cur_co = GetCurrentCoroutine();
    auto pending_co = g_co_call_stack[++g_co_call_stack_top] = co;
    Coroutine::Swap(cur_co, pending_co);
}

Coroutine* PullCoroutine() {
    assert(g_co_call_stack_top > 0);
    auto cur_co = GetCurrentCoroutine();
    auto pending_co = g_co_call_stack[--g_co_call_stack_top];
    Coroutine::Swap(cur_co, pending_co);
    return cur_co;
}

Coroutine* GetCurrentCoroutine() {
    if (g_co_call_stack_top == -1) {
        // ��ʼ����ǰΪ��Э��
        auto main_co = new Coroutine(nullptr);
        main_co->state_ = Coroutine::State::kStExec;
        g_co_call_stack[++g_co_call_stack_top] = main_co;
    }
    return g_co_call_stack[g_co_call_stack_top];
}

void Coroutine::SysContext::Init(char *_ss_sp, size_t _ss_size, void *cb, void *arg0, void *arg1) {
    ss_sp = _ss_sp;
    ss_size = _ss_size;
    // ��ȡջ�׵�ַ(����ŷ��ص�ַ)
    char* sp = ss_sp + ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);

    // ��ջ�״�ŵķ��ص�ַ��ֵΪpfn
    memset(regs, 0, sizeof(regs));
    void** ret_addr = (void**)(sp);
    *ret_addr = (void*)cb;

    regs[kRiRsp] = sp;
    regs[kRiRetAddr] = (char*)cb;
    regs[kRiRdi] = (char*)arg0;
    regs[kRiRsi] = (char*)arg1;
}

void Coroutine::SysContext::Swap(Coroutine::SysContext *new_sys_ctx) {
    sys_context_swap(this, new_sys_ctx);
}

Coroutine::Coroutine(CbType* cb, void* cb_arg, int stack_size, Coroutine::StackMem *stack_mem) {
    if (stack_mem) {
        stack_mem_ = stack_mem;
        is_share_stack_mem_ = true;
    }else {
        if(stack_size & 0xFFF )
        {
            stack_size &= ~0xFFF;
            stack_size += 0x1000;
        }
        stack_mem_ = new StackMem(stack_size);
        is_share_stack_mem_ = false;
    }
    cb_ = cb;
    cb_arg_ = cb_arg;
    sys_context_.Init(stack_mem_->buffer, stack_mem_->size, (void*)&OnCoroutine, this, cb_arg);
    state_ = State::kStInit;
}

Coroutine::~Coroutine() {
    if (!is_share_stack_mem_ && stack_mem_) {
        delete stack_mem_;
        stack_mem_ = nullptr;
    }
    if (stack_backup_buffer) {
        free(stack_backup_buffer);
        stack_backup_buffer = nullptr;
    }
}

void* Coroutine::OnCoroutine(Coroutine* co) {
    co->state_ = State::kStExec;
    co->cb_(co->cb_arg_);
    co->state_ = State::kStEnd;
    Yield();
    assert(false); //���ᵽ������
    return nullptr;
}

void Coroutine::Resume() {
    PushCoroutine(this);
}

void Coroutine::Yield() {
    PullCoroutine();
}

void Coroutine::Swap(Coroutine* cur_co, Coroutine* pending_co) {
    cur_co->state_ = State::kStHold;
    pending_co->state_ = State::kStExec;
    // ��ȡջ��ָ��
    char c;
    cur_co->stack_sp_ = &c;
    if (pending_co->is_share_stack_mem_) {
        // ��Э�̵Ĺ���ջ��ʹ����
        auto share_stack_occupy_co = pending_co->stack_mem_->occupy_co;
        pending_co->stack_mem_->occupy_co = pending_co;
        g_occupy_co = share_stack_occupy_co;
        g_pending_co = pending_co;
        if (share_stack_occupy_co && share_stack_occupy_co != pending_co) {
            share_stack_occupy_co->BackupStackMem();
        }
    }else {
        g_pending_co = nullptr;
        g_occupy_co = nullptr;
    }

    // �л�ϵͳ������
    cur_co->sys_context_.Swap(&pending_co->sys_context_);

    // ��ԭ����ջ
    if (g_occupy_co && g_pending_co && g_occupy_co != g_pending_co && g_pending_co->stack_backup_buffer) {
        memcpy(g_pending_co->stack_sp_, g_pending_co->stack_backup_buffer, g_pending_co->stack_backup_size);
    }
}

void Coroutine::BackupStackMem() {
    int stack_use_len = stack_mem_->bp - stack_sp_;
    if (stack_backup_buffer) {
        free(stack_backup_buffer);
        stack_backup_buffer = nullptr;
    }
    stack_backup_buffer = (char*)malloc(stack_use_len);
    stack_backup_size = stack_use_len;
    memcpy(stack_backup_buffer, stack_sp_, stack_backup_size);
}

XCO_NAMESPAVE_END