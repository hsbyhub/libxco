/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：coroutine.h
      	创 建 者：hsby
      	创建日期：2022/1/27
 *================================================================*/
#pragma once

#include <cstddef>
#include "define.h"

X_NAMESPAVE_START

struct SysContext;
extern "C" {
    void sys_context_swap(SysContext* cur_ctx, SysContext* new_ctx) asm("sys_context_swap");
}

class Coroutine {
public:
    using callback_fn_t = void*(void* arg);

    /**
     * @brief 协程系统上下文
     */
    struct SysContext {
        enum RegIdx{
            kRiRdi = 7,
            kRiRsi = 8,
            kRiRetAddr = 9,
            kRiRsp = 13,
        };
        SysContext(callback_fn_t* cb);
        void* regs[14];
        size_t ss_size; // 栈大小
        char* ss_sp;    // 栈内存块
    };

    /**
     * @brief 运行时栈内存
     */
    struct RunningStackMem{
        Coroutine* current_co;  // 当前使用内存块的协程
        size_t size;            // 使用大小
        char* buffer;           // 内存块起始地址
        char* bp;               // 栈底地址, bp = buffer + size
    };

private:
    SysContext sys_context_;            // 协程上下文
    RunningStackMem running_stack_mem_; // 运行时栈内存
};

X_NAMESPAVE_END