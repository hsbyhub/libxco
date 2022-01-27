/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�coroutine.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/1/27
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
     * @brief Э��ϵͳ������
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
        size_t ss_size; // ջ��С
        char* ss_sp;    // ջ�ڴ��
    };

    /**
     * @brief ����ʱջ�ڴ�
     */
    struct RunningStackMem{
        Coroutine* current_co;  // ��ǰʹ���ڴ���Э��
        size_t size;            // ʹ�ô�С
        char* buffer;           // �ڴ����ʼ��ַ
        char* bp;               // ջ�׵�ַ, bp = buffer + size
    };

private:
    SysContext sys_context_;            // Э��������
    RunningStackMem running_stack_mem_; // ����ʱջ�ڴ�
};

X_NAMESPAVE_END