/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：coroutine.h
      	创 建 者：hsby
      	创建日期：2022/1/27
 *================================================================*/
#pragma once

#include <cstddef>
#include <memory>
#include "common.h"

XCO_NAMESPAVE_START

class Coroutine : public std::enable_shared_from_this<Coroutine>{
public:
    using Ptr = std::shared_ptr<Coroutine>;
    using CbType = std::function<void()>;

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
        SysContext(){}
        void Init(char* _ss_sp,
                   size_t _ss_size,
                   void* cb,
                   void* arg0 = nullptr,
                   void* arg1 = nullptr);

        void Swap(SysContext* new_sys_ctx);

        char* regs[14];
        size_t ss_size; // 栈大小
        char* ss_sp;    // 栈内存块
    };

    /**
     * @brief 运行时栈内存
     */
    struct StackMem{
        StackMem(int _size = 128 * 1000) {
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
        Coroutine*      occupy_co   = nullptr;  // 当前使用内存块的协程
        size_t          size        = 0;        // 使用大小
        char*           buffer      = nullptr;  // 内存块起始地址
        char*           bp          = nullptr;  // 栈底地址, bp = buffer + size
    };

private:
    /**
     * @brief 构造函数
     */
    Coroutine(CbType cb = nullptr, /*void* arg = nullptr, */StackMem* stack_mem = nullptr, int stack_size = 128 * 1024);

public:
    template<typename ...ArgsType>
    static Ptr Create(ArgsType ...args) {
        return std::shared_ptr<Coroutine>(new Coroutine(args...));
    }

    /**
     * @brief 析构函数
     */
    ~Coroutine();

    /**
     * @brief 协程处理
     */
    static void OnCoroutine(Coroutine* co);

    /**
     * @brief 唤起协程
     */
    void Resume();

    /**
     * @brief 挂起协程
     */
    static void Yield();

    /**
     * @brief 获取当前协程
     */
    static Coroutine::Ptr GetCurCoroutine();

    /**
     * @brief 备份栈内存
     */
    void BackupStackMem();

public:
    StackMem*           stack_mem_          = nullptr;  // 栈内存
    bool                is_share_stack_mem_ = false;    // 是否共享栈
    CbType              cb_                 = nullptr;  // 回调
    State               state_;                         // 协程状态
    SysContext          sys_context_;                   // 协程系统上下文
    char*               stack_sp_           = nullptr;  // 栈指针(共享栈的备份)
    int                 stack_backup_size   = 0;        // 栈备份大小
    char*               stack_backup_buffer = nullptr;  // 栈备份
};

XCO_NAMESPAVE_END