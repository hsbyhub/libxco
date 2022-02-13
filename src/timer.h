/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�timer.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
 *================================================================*/
#pragma once

#include "common.h"
#include "coroutine.h"
#include <memory>
#include <set>
#include <vector>
#include <ostream>

XCO_NAMESPAVE_START

/**
 * @brief ��ʱ���¼���
 */
class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>{
    friend class TimerManager;

public:
    typedef std::shared_ptr<Timer> Ptr;

private:
    /**
     * @brief ��ʱ���¼����캯���� ˽�У�ֻ����TimerManager����
     * @param[in] ms        ���룬��ʱ���Ĵ���ʱ��
     * @param[in] cb        �����¼�
     * @param[in] recurring �Ƿ��δ���
     * @param[in] manager   ������
     */
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

    /**
     * @brief ����һ��ֻ��m_next���¼���������ʱ�Ա�
     */
    Timer(uint64_t next);

    /**
     * @brief �Ƚ���
     */
    struct Comparetor {
        bool operator()(const Timer::Ptr& l, const Timer::Ptr& r) const;
    };

public:
    /**
     * @brief ȡ����ʱ���¼�
     */
    bool Cancel();

    /**
     * @brief ˢ�¼�ʱ���¼�ʱ��
     */
    bool Refresh();

    /**
     * @brief ���ü�ʱ���¼�ʱ��
     */
    bool Reset(uint64_t ms, bool from_now = true);
private:

    uint64_t interval_ms_ = 0; //��ʱ������ʱ����ʱ��
    std::function<void()> m_cb; //�¼��ص�
    bool is_recurring_ = false; // �Ƿ��δ���
    uint64_t next_trigger_time_ms_ = 0; //��һ�δ�����ʱ��
    TimerManager* timer_manager_; //��ʱ���¼�������
};

class TimerManager : public std::enable_shared_from_this<TimerManager>{
    friend class Timer;

public:
    typedef std::shared_ptr<TimerManager> Ptr;

    /**
     * @brief ��ʱ�����������캯��
     */
    TimerManager();

    /**
     * @brief ��ʱ����������������
     * @details virtual
     */
    virtual ~TimerManager();

    /**
     * @brief ��Ӽ�ʱ���¼�
     */
    Timer::Ptr AddTimer(uint64_t ms, std::function<void()> cb,
                        bool recurring = false);

    /**
     * @brief ���������ʱ���¼�
     */
    Timer::Ptr AddConditionTimer(uint64_t ms, std::function<void()> cb,
                                 std::weak_ptr<void> cond_weak,
                                 bool recurring = false);

    /**
     * @brief ��ȡ��һ����ʱ���¼���ʱ��
     */
    int64_t GetNextTimerDistNow();

    /**
     * @brief �����Ѿ���ʱ�ļ�ʱ���¼��ص�
     * @param[out] cbs ��ʱ�Ļص�
     */
    void ListExpriredCb(std::vector<std::function<void()>>& cbs);

    /**
     * @brief �жϵ�ǰ�Ƿ��ж�ʱ���¼�
     */
    bool HasTimer();

    /**
     * @brief ��Ӽ�ʱ���¼�
     */
    void AddTimer(Timer::Ptr timer/*, RWMutexType::WriteLockGuardType& wlock*/);

private:
    /**
     * @brief todo
     */
    bool DetectClockRollover(uint64_t now_ms);

private:
    std::set<Timer::Ptr , Timer::Comparetor> timer_list_;
    bool is_tickled_ = false;
    uint64_t last_detect_rollover_time_ = 0;
};

XCO_NAMESPAVE_END