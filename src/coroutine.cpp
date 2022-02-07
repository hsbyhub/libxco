/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：coroutine.cpp
      	创 建 者：hsby
      	创建日期：2022/1/27
 *================================================================*/
#include "coroutine.h"
#include <cstring>
#include <assert.h>

X_NAMESPAVE_START
/**
 * @brief 协程环境信息
 */
const int   g_co_call_stack_max                     = 128;  // 协程调用栈大小
Coroutine*  g_co_call_stack[g_co_call_stack_max]    = {0};  // 协程调用栈
int         g_co_call_stack_top                     = -1;   // 协程调用栈栈顶
Coroutine*  g_pending_co                            = nullptr;
Coroutine*  g_occupy_co                             = nullptr;

/**
 * @brief 协程入栈
 */
Coroutine* PullCoroutine();

/**
 * @brief 协程出栈
 */
void PushCoroutine(Coroutine* co);

/**
 * @brief 获取当前协程
 */
Coroutine* GetCurrentCoroutine();

Coroutine::SysContext::SysContext(char* _ss_sp,
           size_t _ss_size,
           void* cb,
           void* arg0,
           void* arg1) : ss_sp(_ss_sp), ss_size(_ss_size){

    // 获取栈底地址(存放着返回地址)
    char* sp = ss_sp + ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);

    // 将栈底存放的返回地址赋值为pfn
    memset(regs, 0, sizeof(regs));
    void** ret_addr = (void**)(sp);
    *ret_addr = (void*)cb;

    regs[kRiRsp] = sp;
    regs[kRiRetAddr] = ret_addr;
    regs[kRiRdi] = arg0;
    regs[kRiRsi] = arg1;
}

void Coroutine::SysContext::Swap(Coroutine::SysContext *new_sys_ctx) {
}

void PushCoroutine(Coroutine* co) {
    assert(g_co_call_stack_top < g_co_call_stack_max - 1);
    auto cur_co = g_co_call_stack[g_co_call_stack_top];
    auto pending_co = g_co_call_stack[++g_co_call_stack_top] = co;
    Coroutine::Swap(cur_co, pending_co);
}

Coroutine* PullCoroutine() {
    assert(g_co_call_stack_top > -1);
    auto cur_co = g_co_call_stack[g_co_call_stack_top];
    auto pending_co = g_co_call_stack[--g_co_call_stack_top];
    Coroutine::Swap(cur_co, pending_co);
    return cur_co;
}

Coroutine* GetCurrentCoroutine() {
    if (g_co_call_stack_top == -1) {
        // 初始化当前为主协程
        auto cur_co = new Coroutine(nullptr);
        cur_co->SetState(Coroutine::State::kStExec);
        PushCoroutine(cur_co);
    }
    return g_co_call_stack[g_co_call_stack_top];
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
    state_ = State::kStInit;
}

Coroutine::~Coroutine() {
    if (!is_share_stack_mem_ && stack_mem_) {
        delete stack_mem_;
        stack_mem_ = nullptr;
    }
    if (sys_context_) {
        delete sys_context_;
        sys_context_ = nullptr;
    }
    if (stack_backup_buffer) {
        free(stack_backup_buffer);
        stack_backup_buffer = nullptr;
    }
}

void* Coroutine::OnCoroutine(void *arg) {
    assert(state_ == State::kStExec);
    void* ret = cb_(arg);
    state_ = State::kStEnd;
    return ret;
}

void Coroutine::Resume() {
    assert(state_ != State::kStEnd);
    if (!sys_context_) {
        sys_context_ = new SysContext(stack_mem_->buffer, stack_mem_->size, (void*)cb_, this, cb_arg_);
    }
    PushCoroutine(this);
}

void Coroutine::Yield() {
    PullCoroutine();
}

void Coroutine::Swap(Coroutine* cur_co, Coroutine* pending_co) {
    cur_co->SetState(State::kStHold);
    pending_co->SetState(State::kStExec);
    // 获取栈顶指针
    char c;
    cur_co->stack_sp_ = &c;
    if (pending_co->is_share_stack_mem_) {
        // 新协程的共享栈的使用者
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

    // 切换系统上下文
    sys_context_swap(cur_co->sys_context_, pending_co->sys_context_);

    // 还原共享栈
    if (g_occupy_co && g_pending_co && g_occupy_co != g_pending_co && g_pending_co->stack_backup_buffer) {
        memcpy(g_pending_co->stack_sp_, g_pending_co->stack_backup_buffer, g_pending_co->stack_backup_size);
    }
}

Coroutine::State Coroutine::GetState() {
    return state_;
}

void Coroutine::SetState(Coroutine::State st) {
    state_ = st;
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

X_NAMESPAVE_END