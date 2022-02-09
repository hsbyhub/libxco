/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�scheduler.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
 *================================================================*/
#pragma once

#include "coroutine.h"
#include <list>

XCO_NAMESPAVE_START

class Scheduler {
public:
    /**
     * @brief ���캯��
     */
    Scheduler();

    /**
     * @brief ��������
     */
    virtual ~Scheduler();

public:
    /**
     * @brief ��ʼ
     */
    virtual void Start();

protected:
    /**
     * @brief ��ѭ��
     */
    virtual void OnLoop();

    /**
     * @brief ���ô���
     */
    virtual void OnIdle();

    /**
     * @brief ��ѭ���ص�
     */
    static void LoopCb(void* arg);

    /**
     * @brief ���ô����ص�
     */
    static void IdleCb(void* arg);

public:
    /**
     * @brief ��ȡ��ǰ������
     */
    static Scheduler* GetCurScheduler();

public:
    /**
     * @brief ����
     * @param[in] coc Э�̻�ص�
     */
    void Schedule(Coroutine* co);

private:
    Coroutine*                  loop_co_    = nullptr;  // ��ѭ��Э��
    Coroutine*                  idle_co_    = nullptr;  // ����Э��
    std::list<Coroutine*>       co_list_;
};

XCO_NAMESPAVE_END