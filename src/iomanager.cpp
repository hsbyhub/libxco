/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：iomanager.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#include "iomanager.h"
#include <assert.h>
#include <unistd.h>
#include <cstring>

const static int MAX_EPOLL_TIMEOUT_MS = 2000;
const static int MAX_EPOLL_RET_EPEV_CNT = 256;

XCO_NAMESPAVE_START

IoManager::IoManager() {
    epoll_fd_ = epoll_create(1);
    assert(epoll_fd_ > 0);
}

IoManager::~IoManager() {
    close(epoll_fd_);
}

uint32_t IoManager::SetEvent(int fd, uint32_t ev, Coroutine *co) {
    assert(fd > 0 && co);
    if (fd >= (int)fd_ctxs_.size()) {
        fd_ctxs_.resize(fd * 2);
    }

    if (ev & EPOLLIN) {
        ev = EPOLLIN;
    }else if (ev & EPOLLOUT) {
        ev = EPOLLOUT;
    }else {
        return 0;
    }

    // 修改epoll
    auto& ctx = fd_ctxs_[fd];
    int op = ctx.ev_flags ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epev;
    memset(&epev, 0, sizeof(epev));
    epev.events = ctx.ev_flags | ev | EPOLLET;
    epev.data.ptr = &ctx;
    if (epoll_ctl(epoll_fd_, op, fd, &epev)) {
        return 0;
    }

    // 修改fd_ctx
    if (ev & EPOLLIN) {
        ctx.read_co = co;
    }else if (ev & EPOLLOUT){
        ctx.write_co = co;
    }
    ctx.ev_flags |= ev;
    ctx.fd = fd;
    return ev;
}

uint32_t IoManager::TrgEvent(int fd, uint32_t evs) {
    assert(fd < (int)fd_ctxs_.size());
    evs = DelEvent(fd, evs);
    auto& ctx = fd_ctxs_[fd];
    if (evs & EPOLLIN && ctx.ev_flags & EPOLLIN && ctx.read_co) {
        Schedule(ctx.read_co);
        ctx.read_co = nullptr;
    }else if (evs & EPOLLOUT && ctx.ev_flags & EPOLLOUT && ctx.write_co){
        Schedule(ctx.write_co);
        ctx.write_co = nullptr;
    }
    return evs;
}

uint32_t IoManager::DelEvent(int fd, uint32_t evs) {
    assert(fd < (int)fd_ctxs_.size());
    auto& ctx = fd_ctxs_[fd];
    uint32_t remain_evs = ctx.ev_flags & ~evs;
    int op = remain_evs ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epev;
    memset(&epev, 0, sizeof(epev));
    epev.events = remain_evs | EPOLLET;
    epev.data.ptr = &ctx;
    if (epoll_ctl(epoll_fd_, op, fd, &epev)) {
        return 0;
    }

    ctx.ev_flags = remain_evs;
    return evs;
}

IoManager *IoManager::GetCurIoManager() {
    return dynamic_cast<IoManager*>(GetCurScheduler());
}

void IoManager::OnIdle() {
    epoll_event ret_epevs[MAX_EPOLL_RET_EPEV_CNT];
    while(true) {
        int ret = epoll_wait(epoll_fd_, ret_epevs, MAX_EPOLL_RET_EPEV_CNT, MAX_EPOLL_TIMEOUT_MS);
        LOGDEBUG("epoll_wait ret = " << ret);
        for (int i = 0; i < ret; ++i) {
            const auto& ret_epev = ret_epevs[i];
            auto ctx = (FdContext*)ret_epev.data.ptr;
            if (!ctx) {
                continue;
            }
            TrgEvent(ctx->fd, ret_epev.events);
        }
        Coroutine::Yield();
    }
}

XCO_NAMESPAVE_END