/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�scheduler.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
 *================================================================*/
#pragma once

#include "coroutine.h"
#include <queue>

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
    void OnLoop();

    /**
     * @brief ���ô���
     */
    virtual void OnIdle();

public:
    /**
     * @brief ��ȡ��ǰ������
     */
    static Scheduler* GetCurScheduler();

    /**
     * @brief ����
     * @param[in] coc Э�̻�ص�
     */
    static void Schedule(Coroutine::Ptr co);

private:
    Coroutine::Ptr                 loop_co_    = nullptr;  // ��ѭ��Э��
    Coroutine::Ptr                 idle_co_    = nullptr;  // ����Э��
    std::queue<Coroutine::Ptr>       co_list_;
};

XCO_NAMESPAVE_END