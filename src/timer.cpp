/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：timer.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#include "timer.h"

XCO_NAMESPAVE_START

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager)
    : interval_ms_(ms)
    , m_cb(cb)
    , is_recurring_(recurring)
    , timer_manager_(manager){
    next_trigger_time_ms_ = TimeStampMs() + interval_ms_;
}

Timer::Timer(uint64_t next) : next_trigger_time_ms_(next){}

bool Timer::Comparetor::operator()(const Timer::Ptr &l, const Timer::Ptr &r) const {
    if (!l || !r){
        if (!l)
            return true;
        else {
            return false;
        }
    }
    return l->next_trigger_time_ms_ < r->next_trigger_time_ms_ || (l->next_trigger_time_ms_ == r->next_trigger_time_ms_ && l.get() < r.get());
}

bool Timer::Cancel() {
    if (m_cb) {
        timer_manager_->timer_list_.erase(shared_from_this());
        return true;
    }
    return false;
}

bool Timer::Refresh() {
    if (!m_cb) {
        return false;
    }
    auto it = timer_manager_->timer_list_.find(shared_from_this());
    auto self = *it;
    if (it == timer_manager_->timer_list_.end())
        return false;
    timer_manager_->timer_list_.erase(it);
    next_trigger_time_ms_ = TimeStampMs() + interval_ms_;
    timer_manager_->timer_list_.insert(self);
    return true;
}

bool Timer::Reset(uint64_t ms, bool from_now) {
    if (!from_now && ms == interval_ms_) {
        return true;
    }
    if (!m_cb) {
        return false;
    }

    auto it = timer_manager_->timer_list_.find(shared_from_this());
    Timer::Ptr self = *it;
    if (it == timer_manager_->timer_list_.end())
        return false;

    timer_manager_->timer_list_.erase(it);
    uint64_t start = from_now ? TimeStampMs() : next_trigger_time_ms_ - interval_ms_;
    interval_ms_ = ms;
    next_trigger_time_ms_ = start + interval_ms_;
    timer_manager_->AddTimer(self/*, wlock*/);

    return true;
}

TimerManager::TimerManager() {

}

TimerManager::~TimerManager() {

}

Timer::Ptr TimerManager::AddTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::Ptr timer(new Timer(ms, cb, recurring, this));
    AddTimer(timer/*, wlock*/);
    return timer;
}

void TimerManager::AddTimer(Timer::Ptr timer) {
    auto it = timer_list_.insert(timer).first;
    bool insert_at_front = it == timer_list_.begin() && !is_tickled_;

    if (insert_at_front) {
        is_tickled_ = true;
    }
}

static void TimerConditionCb(std::weak_ptr<void> cond_weak, std::function<void()> cb) {
    std::shared_ptr<void> cond = cond_weak.lock();
    if (cond)
        cb();
}

Timer::Ptr TimerManager::AddConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond_weak,
                                           bool recurring) {
    return AddTimer(ms, std::bind(&TimerConditionCb, cond_weak, cb), recurring);
}

int64_t TimerManager::GetNextTimerDistNow() {
    is_tickled_ = false;
    if (timer_list_.empty()) {
        return -1;
    }

    // 取得距离现在最近计时器的时间
    const Timer::Ptr& nextTimer = *timer_list_.begin();
    uint64_t now_ms = TimeStampMs();
    // 已经超时
    if (now_ms >= nextTimer->next_trigger_time_ms_) {
        return 0;
    }
    // 返回最近计时器事件距离现在毫秒时间
    return nextTimer->next_trigger_time_ms_ - now_ms;
}

void TimerManager::ListExpriredCb(std::vector<std::function<void()>>& cbs) {
    uint64_t now_ms = TimeStampMs();
    {
        if (timer_list_.empty())
            return;
    }

    bool rollover = DetectClockRollover(now_ms);
    if (!rollover && (*timer_list_.begin())->next_trigger_time_ms_ > now_ms) {
        return;
    }

    std::vector<Timer::Ptr> exprired_timers;
    Timer::Ptr now_timer(new Timer(now_ms));
    auto end = rollover ? timer_list_.end() : timer_list_.upper_bound(now_timer);
    for(auto it = timer_list_.begin(); it != end; it++) {
        exprired_timers.push_back(*it);
    }
    timer_list_.erase(timer_list_.begin(), end);

    cbs.reserve(exprired_timers.size());
    for(auto& it : exprired_timers) {
        cbs.push_back(it->m_cb);
        if (it->is_recurring_) {
            it->next_trigger_time_ms_ = now_ms + it->interval_ms_;
            timer_list_.insert(it);
        }else {
            it->m_cb = nullptr;
        }
    }
}

bool TimerManager::HasTimer() {
    return !timer_list_.empty();
}

bool TimerManager::DetectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < last_detect_rollover_time_ && now_ms < (last_detect_rollover_time_ - 60 * 60 * 1000)) {
        rollover = true;
    }
    last_detect_rollover_time_ = now_ms;
    return rollover;
}

XCO_NAMESPAVE_END