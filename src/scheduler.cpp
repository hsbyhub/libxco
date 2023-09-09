/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：scheduler.cpp
      	创 建 者：xiesenxin
      	创建日期：2022/2/9
 *================================================================*/
#include "scheduler.h"
#include "hook.h"

XCO_NAMESPAVE_START

static thread_local Scheduler* t_scheduler = nullptr;

Scheduler::Scheduler() {
    t_scheduler = this;
    loop_co_ = Coroutine::Create(std::bind(&Scheduler::OnLoop, this));
    idle_co_ = Coroutine::Create(std::bind(&Scheduler::OnIdle, this));
}

Scheduler::~Scheduler() {
}

void Scheduler::Start() {
    loop_co_->Resume();
}

void Scheduler::OnLoop() {
    SetHookEnable(true);
    while(true) {
        while(!co_list_.empty()) {
            auto co = co_list_.front();
            co_list_.pop();
            if (!co || co->state_ == Coroutine::State::kStEnd) {
                continue;
            }
            co->Resume();
        }
        idle_co_->Resume();
        if (idle_co_->state_ == Coroutine::State::kStEnd) {
            break;
        }
    }
}

void Scheduler::OnIdle() {
    while(true) {
        Coroutine::Yield();
    }
}

Scheduler *Scheduler::GetCurScheduler() {
    return t_scheduler;
}

void Scheduler::Schedule(Coroutine::Ptr co) {
    auto sche = GetCurScheduler();
    if (!sche) {
        return;
    }
    sche->co_list_.push(co);
}

XCO_NAMESPAVE_END