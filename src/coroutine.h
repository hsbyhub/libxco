/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：coroutine.h
      	创 建 者：hsby
      	创建日期：2022/1/27
 *================================================================*/
#pragma once

#include <cstddef>
#include <memory>
#include "define.h"

X_NAMESPAVE_START

class Coroutine {
public:
    using CbType = void*(void* arg);

    enum class State{
        kStInit     = 0,
        kStExec     = 1,
        kStHold     = 2,
        kStEnd      = 3,
    };

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
        SysContext(char* _ss_sp,
                   size_t _ss_size,
                   void* cb,
                   void* arg0 = nullptr,
                   void* arg1 = nullptr);

        void Swap(SysContext* new_sys_ctx);

        void* regs[14];
        char* ss_sp;    // 栈内存块
        size_t ss_size; // 栈大小
    };

    /**
     * @brief 运行时栈内存
     */
    struct StackMem{
        StackMem(int _size) {
            occupy_co = nullptr;
            size = _size;
            buffer = (char*)malloc(size);
            bp = buffer + size;
        }
        ~StackMem() {
            if (buffer) {
                free(buffer);
                buffer = nullptr;
            }
        }
        Coroutine* occupy_co    = nullptr;  // 当前使用内存块的协程
        size_t size             = 0;        // 使用大小
        char* buffer            = nullptr;  // 内存块起始地址
        char* bp                = nullptr;  // 栈底地址, bp = buffer + size
    };

public:
    /**
     * @brief 构造函数
     */
    Coroutine(CbType* cb, void* arg = nullptr, int stack_size = 128 * 1024, StackMem* stack_mem = nullptr);

    /**
     * @brief 析构函数
     */
    ~Coroutine();

    /**
     * @brief 协程处理
     */
    void* OnCoroutine(void* arg);

    /**
     * @brief 唤起协程
     */
    void Resume();

    State GetState();

    void SetState(State st);
    
    /**
     * @brief 挂起协程
     */
    static void Yield();
    
    /**
     * @brief 切换协程
     */
    static void Swap(Coroutine* cur_co, Coroutine* pending_co);

private:
    void BackupStackMem();

private:
    StackMem*           stack_mem_          = nullptr;  // 栈内存
    bool                is_share_stack_mem_ = false;    // 是否共享栈
    SysContext*         sys_context_        = nullptr;  // 协程上下文
    CbType*             cb_                 = nullptr;  // 回调
    void*               cb_arg_             = nullptr;  // 回调参数
    State               state_;                         // 协程状态
    char*               stack_sp_           = nullptr;  // 栈指针(共享栈的备份)
    int                 stack_backup_size   = 0;        // 栈备份大小
    char*               stack_backup_buffer = nullptr;  // 栈备份
};

extern "C" {
void sys_context_swap(xco::Coroutine::SysContext* cur_ctx, xco::Coroutine::SysContext* new_ctx) asm("sys_context_swap");
}

X_NAMESPAVE_END