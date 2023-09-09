/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：iomanager.h
      	创 建 者：xiesenxin
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

#include "scheduler.h"
#include "timer.h"
#include <sys/epoll.h>
#include <vector>

XCO_NAMESPAVE_START

class IoManager : public Scheduler, public TimerManager{
public:
    struct FdContext {
        int fd                      = -1;
        Coroutine::Ptr read_co      = nullptr;
        Coroutine::Ptr write_co     = nullptr;
        uint32_t evs                = 0;

        void SetEv(int f, uint32_t ev, Coroutine::Ptr co) {
            if (ev & EPOLLIN) {
                read_co = co;
            }
            if (ev & EPOLLOUT) {
                write_co = co;
            }
            evs |= ev;
            fd = f;
        }

        void TrgEv(uint32_t ev) {
            if (ev & EPOLLIN) {
                XCO_ASSERT(evs & EPOLLIN);
                IoManager::Schedule(read_co);
                read_co.reset();
            }
            if (ev & EPOLLOUT) {
                XCO_ASSERT(evs & EPOLLOUT);
                IoManager::Schedule(write_co);
                write_co.reset();
            }
            evs &= ~ev;
        }

        void DelEv(uint32_t ev) {
            if (ev & EPOLLIN) {
                XCO_ASSERT(evs & EPOLLIN);
                read_co.reset();
            }
            if (ev & EPOLLOUT) {
                XCO_ASSERT(evs & EPOLLOUT);
                write_co.reset();
            }
            evs &= ~ev;
        }

    };

public:
    IoManager();

    ~IoManager() override;

public:
    uint32_t SetEvent(int fd, uint32_t ev, Coroutine::Ptr co);

    uint32_t TrgEvent(int fd, uint32_t evs);

    uint32_t DelEvent(int fd, uint32_t evs);

    uint32_t CncAllEvent(int fd);

    static IoManager* GetCurIoManager();

protected:
    void OnIdle() override;

private:
    int epoll_fd_ = -1;
    std::vector<FdContext> fd_ctxs_;
};

XCO_NAMESPAVE_END