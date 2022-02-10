/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：scheduler.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#include "scheduler.h"
#include "hook.h"

XCO_NAMESPAVE_START

static thread_local Scheduler* t_scheduler = nullptr;

Scheduler::Scheduler() {
    t_scheduler = this;
    loop_co_ = new Coroutine(LoopCb, this);
    idle_co_ = new Coroutine(IdleCb, this);
}

Scheduler::~Scheduler() {
    if (loop_co_) {
        delete loop_co_;
        loop_co_ = nullptr;
    }
    if (idle_co_) {
        delete idle_co_;
        idle_co_ = nullptr;
    }
    for (auto co : co_list_) {
        if (co) {
            delete co;
        }
    }
}

void Scheduler::Start() {
    loop_co_->Resume();
}

void Scheduler::OnLoop() {
    SetHookEnable(true);
    while(true) {
        while(!co_list_.empty()) {
            Coroutine* co = co_list_.front();
            co_list_.pop_front();
            if (!co) {
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

void Scheduler::LoopCb(void* arg) {
    ((Scheduler*)arg)->OnLoop();
}

void Scheduler::IdleCb(void* arg) {
    ((Scheduler*)arg)->OnIdle();
}

Scheduler *Scheduler::GetCurScheduler() {
    return t_scheduler;
}

void Scheduler::Schedule(Coroutine* co) {
    co_list_.push_back(co);
}

XCO_NAMESPAVE_END