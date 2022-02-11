/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�iomanager.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
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
        int fd = -1;
        Coroutine::Ptr read_co      = nullptr;
        Coroutine::Ptr write_co     = nullptr;
        uint32_t ev_flags       = 0;
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
    int pending_fd_cnt = 0;
};

XCO_NAMESPAVE_END