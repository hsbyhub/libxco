/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：coroutine.cpp
      	创 建 者：hsby
      	创建日期：2022/1/27
 *================================================================*/
#include "coroutine.h"
#include <cstring>
#include <assert.h>
#include <stack>

extern "C" {
void sys_context_swap(xco::Coroutine::SysContext* cur_ctx, xco::Coroutine::SysContext* new_ctx) asm("sys_context_swap");
}

XCO_NAMESPAVE_START

const int   g_co_call_stack_max                     = 128;  // 协程调用栈大小

int co_cnt = 0;

/**
 * @brief 协程环境信息
 */
struct CoroutineEnv {
    std::stack<Coroutine::Ptr> co_call_stack;
    Coroutine* occupy_co;
    Coroutine* pending_co;

    void Swap(Coroutine* cco, Coroutine* pco) {
        cco->state_ = Coroutine::State::kStHold;
        pco->state_ = Coroutine::State::kStExec;

        // 获取栈顶指针
        char c;
        cco->stack_sp_ = &c;
        if (pco->is_share_stack_mem_) {
            // 新协程的共享栈的使用者
            occupy_co = pco->stack_mem_->occupy_co;
            pending_co = pco;
            pco->stack_mem_->occupy_co = pco;
            if (occupy_co && occupy_co != pco) {
                occupy_co->BackupStackMem();
            }
        }else {
            occupy_co = nullptr;
            pending_co = nullptr;
        }

        // 切换系统上下文
        cco->sys_context_.Swap(&pco->sys_context_);

        // 还原共享栈
        if (occupy_co
            && pending_co
            && occupy_co != pending_co
            && pending_co->stack_backup_buffer) {
            memcpy(pending_co->stack_sp_, pending_co->stack_backup_buffer, pending_co->stack_backup_size);
        }
    }

    void PushCoroutine(Coroutine::Ptr co) {
        if (co_call_stack.size() >= g_co_call_stack_max - 1) {
            throw std::out_of_range("coroutine call stack out of size");
        }
        auto cco = GetCurrentCoroutine();
        co_call_stack.push(co);
        auto pco = co_call_stack.top();
        Swap(cco.get(), pco.get());
    }

    Coroutine* PullCoroutine() {
        if (co_call_stack.size() <= 1) {
            throw std::out_of_range("coroutine call stack out of size");
        }
        auto cco = GetCurrentCoroutine().get();
        co_call_stack.pop();
        auto pco = co_call_stack.top();
        Swap(cco, pco.get());
        return cco;
    }

    Coroutine::Ptr GetCurrentCoroutine() {
        if (co_call_stack.size() == 0) {
            // 初始化当前为主协程
            auto main_co = Coroutine::Create();
            main_co->state_ = Coroutine::State::kStExec;
            co_call_stack.push(main_co);
        }
        return co_call_stack.top();
    }
};
static thread_local CoroutineEnv s_coroutine_env;

void Coroutine::SysContext::Init(char *_ss_sp, size_t _ss_size, void *cb, void *arg0, void *arg1) {
    ss_sp = _ss_sp;
    ss_size = _ss_size;
    // 获取栈底地址(存放着返回地址)
    char* sp = ss_sp + ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);

    // 将栈底存放的返回地址赋值为pfn
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

Coroutine::Coroutine(CbType cb, /*void* cb_arg, */Coroutine::StackMem *stack_mem, int stack_size) {
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
    sys_context_.Init(stack_mem_->buffer, stack_mem_->size, (void*)&OnCoroutine, this, nullptr);
    state_ = State::kStInit;
    co_cnt++;
    LOGDEBUG("Coroutine constructor "<< XCO_VARS_EXP(co_cnt));
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
    co_cnt--;
    LOGDEBUG("Coroutine destructor "<< XCO_VARS_EXP(co_cnt));
}

void Coroutine::OnCoroutine(Coroutine* co) {
    co->state_ = State::kStExec;
    co->cb_(/*co->cb_arg_*/);
    co->state_ = State::kStEnd;
    Yield();
    assert(false); //不会到达这里
}

void Coroutine::Resume() {
    s_coroutine_env.PushCoroutine(shared_from_this());
}

void Coroutine::Yield() {
    s_coroutine_env.PullCoroutine();
}

Coroutine::Ptr Coroutine::GetCurCoroutine() {
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