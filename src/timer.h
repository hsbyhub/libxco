/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：timer.h
      	创 建 者：hsby
      	创建日期：2022/2/9
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
 * @brief 定时器事件类
 */
class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>{
    friend class TimerManager;

public:
    typedef std::shared_ptr<Timer> Ptr;

private:
    /**
     * @brief 定时器事件构造函数， 私有，只能由TimerManager构造
     * @param[in] ms        毫秒，计时器的触发时间
     * @param[in] cb        触发事件
     * @param[in] recurring 是否多次触发
     * @param[in] manager   管理器
     */
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

    /**
     * @brief 构造一个只有m_next的事件，用于临时对比
     */
    Timer(uint64_t next);

    /**
     * @brief 比较器
     */
    struct Comparetor {
        bool operator()(const Timer::Ptr& l, const Timer::Ptr& r) const;
    };

public:
    /**
     * @brief 取消计时器事件
     */
    bool Cancel();

    /**
     * @brief 刷新计时器事件时间
     */
    bool Refresh();

    /**
     * @brief 重置计时器事件时间
     */
    bool Reset(uint64_t ms, bool from_now = true);
private:

    uint64_t interval_ms_ = 0; //定时器构造时触发时间
    std::function<void()> m_cb; //事件回调
    bool is_recurring_ = false; // 是否多次触发
    uint64_t next_trigger_time_ms_ = 0; //下一次触发的时间
    TimerManager* timer_manager_; //计时器事件管理器
};

class TimerManager : public std::enable_shared_from_this<TimerManager>{
    friend class Timer;

public:
    typedef std::shared_ptr<TimerManager> Ptr;

    /**
     * @brief 计时器管理器构造函数
     */
    TimerManager();

    /**
     * @brief 计时器管理器析构函数
     * @details virtual
     */
    virtual ~TimerManager();

    /**
     * @brief 添加计时器事件
     */
    Timer::Ptr AddTimer(uint64_t ms, std::function<void()> cb,
                        bool recurring = false);

    /**
     * @brief 添加条件计时器事件
     */
    Timer::Ptr AddConditionTimer(uint64_t ms, std::function<void()> cb,
                                 std::weak_ptr<void> cond_weak,
                                 bool recurring = false);

    /**
     * @brief 获取下一个计时器事件的时间
     */
    int64_t GetNextTimerDistNow();

    /**
     * @brief 返回已经超时的计时器事件回调
     * @param[out] cbs 超时的回调
     */
    void ListExpriredCb(std::vector<std::function<void()>>& cbs);

    /**
     * @brief 判断当前是否有定时器事件
     */
    bool HasTimer();

    /**
     * @brief 添加计时器事件
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