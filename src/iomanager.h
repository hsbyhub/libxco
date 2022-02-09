/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：iomanager.h
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

#include "scheduler.h"
#include <sys/epoll.h>
#include <vector>

XCO_NAMESPAVE_START

class IoManager : public Scheduler{
public:
    struct FdContext {
        Coroutine* read_co      = nullptr;
        Coroutine* write_co     = nullptr;
        uint32_t ev_flags       = 0;
    };

public:
    IoManager();

    ~IoManager() override;

public:
    uint32_t SetEvent(int fd, uint32_t ev, Coroutine* co);

    uint32_t TrgEvent(int fd, uint32_t evs);

    uint32_t DelEvent(int fd, uint32_t evs);

protected:
    void OnIdle() override;

private:
    int epoll_fd_ = -1;
    std::vector<FdContext> fd_ctxs_;
};

XCO_NAMESPAVE_END