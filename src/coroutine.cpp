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

const int   g_co_call_stack_max                     = 128;  // Э�̵���ջ��С

/**
 * @brief Э�̻�����Ϣ
 */
struct CoroutineEnv {
    Coroutine*  co_call_stack[g_co_call_stack_max]    = {0};  // Э�̵���ջ
    int         co_call_stack_top                     = -1;   // Э�̵���ջջ��
    Coroutine*  pending_co                            = nullptr;
    Coroutine*  occupy_co                             = nullptr;

    void Swap(Coroutine* cco, Coroutine* pco) {
        cco->state_ = Coroutine::State::kStHold;
        pco->state_ = Coroutine::State::kStExec;

        // ��ȡջ��ָ��
        char c;
        cco->stack_sp_ = &c;
        if (pco->is_share_stack_mem_) {
            // ��Э�̵Ĺ���ջ��ʹ����
            auto share_stack_occupy_co = pco->stack_mem_->occupy_co;
            pco->stack_mem_->occupy_co = pco;
            occupy_co = share_stack_occupy_co;
            pending_co = pco;
            if (share_stack_occupy_co && share_stack_occupy_co != pco) {
                share_stack_occupy_co->BackupStackMem();
            }
        }else {
            occupy_co = nullptr;
            pending_co = nullptr;
        }

        // �л�ϵͳ������
        cco->sys_context_.Swap(&pco->sys_context_);

        // ��ԭ����ջ
        if (occupy_co
            && pending_co
            && occupy_co != pending_co
            && pending_co->stack_backup_buffer) {
            memcpy(pending_co->stack_sp_, pending_co->stack_backup_buffer, pending_co->stack_backup_size);
        }
    }

    void PushCoroutine(Coroutine* co) {
        assert(co_call_stack_top < g_co_call_stack_max - 1);
        auto cco = GetCurrentCoroutine();
        auto pco = co_call_stack[++co_call_stack_top] = co;
        Swap(cco , pco);
    }

    Coroutine* PullCoroutine() {
        assert(co_call_stack_top > 0);
        auto cco = GetCurrentCoroutine();
        auto pco = co_call_stack[--co_call_stack_top];
        Swap(cco, pco);
        return cco;
    }

    Coroutine* GetCurrentCoroutine() {
        if (co_call_stack_top == -1) {
            // ��ʼ����ǰΪ��Э��
            auto main_co = new Coroutine(nullptr);
            main_co->state_ = Coroutine::State::kStExec;
            co_call_stack[++co_call_stack_top] = main_co;
        }
        return co_call_stack[co_call_stack_top];
    }
};
static thread_local CoroutineEnv s_coroutine_env;

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

Coroutine::Coroutine(CbType cb, void* cb_arg, Coroutine::StackMem *stack_mem, int stack_size) {
    if (stack_mem) {
        stack_mem_ = stack_mem;
        is_share_stack_mem_ = true;
    }else {
        if(stack_size & 0xFFF)
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

void Coroutine::OnCoroutine(Coroutine* co) {
    co->state_ = State::kStExec;
    co->cb_(co->cb_arg_);
    co->state_ = State::kStEnd;
    Yield();
    assert(false); //���ᵽ������
}

void Coroutine::Resume() {
    s_coroutine_env.PushCoroutine(this);
}

void Coroutine::Yield() {
    s_coroutine_env.PullCoroutine();
}

Coroutine *Coroutine::GetCurCoroutine() {
    return s_coroutine_env.GetCurrentCoroutine();
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