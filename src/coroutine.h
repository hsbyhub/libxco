/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�coroutine.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/1/27
 *================================================================*/
#pragma once

#include <cstddef>
#include <memory>
#include "define.h"

XCO_NAMESPAVE_START

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
     * @brief Э��ϵͳ������
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
        size_t ss_size; // ջ��С
        char* ss_sp;    // ջ�ڴ��
    };

    /**
     * @brief ����ʱջ�ڴ�
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
        Coroutine* occupy_co    = nullptr;  // ��ǰʹ���ڴ���Э��
        size_t size             = 0;        // ʹ�ô�С
        char* buffer            = nullptr;  // �ڴ����ʼ��ַ
        char* bp                = nullptr;  // ջ�׵�ַ, bp = buffer + size
    };

public:
    /**
     * @brief ���캯��
     */
    Coroutine(CbType* cb, void* arg = nullptr, int stack_size = 128 * 1024, StackMem* stack_mem = nullptr);

    /**
     * @brief ��������
     */
    ~Coroutine();

    /**
     * @brief Э�̴���
     */
    void* OnCoroutine(void* arg);

    /**
     * @brief ����Э��
     */
    void Resume();

    /**
     * @brief ����Э��
     */
    static void Yield();
    
    /**
     * @brief �л�Э��
     */
    static void Swap(Coroutine* cur_co, Coroutine* pending_co);

private:
    void BackupStackMem();

public:
    State GetState();

    void SetState(State st);

public:
    StackMem*           stack_mem_          = nullptr;  // ջ�ڴ�
    bool                is_share_stack_mem_ = false;    // �Ƿ���ջ
    SysContext*         sys_context_        = nullptr;  // Э��������
    CbType*             cb_                 = nullptr;  // �ص�
    void*               cb_arg_             = nullptr;  // �ص�����
    State               state_;                         // Э��״̬
    char*               stack_sp_           = nullptr;  // ջָ��(����ջ�ı���)
    int                 stack_backup_size   = 0;        // ջ���ݴ�С
    char*               stack_backup_buffer = nullptr;  // ջ����
};

XCO_NAMESPAVE_END