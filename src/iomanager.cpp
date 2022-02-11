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


XCO_NAMESPAVE_START

IoManager::IoManager() {
    epoll_fd_ = epoll_create(1);
    assert(epoll_fd_ > 0);
}

IoManager::~IoManager() {
    close(epoll_fd_);
}

uint32_t IoManager::SetEvent(int fd, uint32_t ev, Coroutine::Ptr co) {
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
        LOGDEBUG(XCO_FUNC_WITH_ARG_EXP(fd, op, ev));
        assert(false);
        return 0;
    }

    // 修改fd_ctx
    if (ev & EPOLLIN) {
        ctx.read_co = co;
    }else if (ev & EPOLLOUT){
        ctx.write_co = co;
    }
    ctx.fd = fd;
    ctx.ev_flags |= ev;

    if (op == EPOLL_CTL_ADD) {
        fds.insert(fd);
    }

    return ev;
}

uint32_t IoManager::TrgEvent(int fd, uint32_t evs) {
    if (fd < 0 || fd >= (int)fd_ctxs_.size()) {
        return 0;
    }
    auto& ctx = fd_ctxs_[fd];
    uint32_t change_evs = ctx.ev_flags & evs;
    if (!change_evs) {
        return 0;
    }
    uint32_t remain_evs = ctx.ev_flags & ~change_evs;
    int op = remain_evs ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epev;
    memset(&epev, 0, sizeof(epev));
    epev.events = remain_evs | EPOLLET;
    epev.data.ptr = &ctx;
    if (epoll_ctl(epoll_fd_, op, fd, &epev)) {
        LOGFATAL("TrgEvent" << XCO_FUNC_WITH_ARG_EXP(fd, op, evs, change_evs, remain_evs));
        assert(false);
        return 0;
    }

    if (change_evs & EPOLLIN) {
        assert(ctx.read_co);
        Schedule(ctx.read_co);
        ctx.read_co.reset();
    }
    if (change_evs & EPOLLOUT){
        assert(ctx.write_co);
        Schedule(ctx.write_co);
        ctx.write_co.reset();
    }
    ctx.ev_flags = remain_evs;

    if (op == EPOLL_CTL_DEL) {
        fds.erase(fd);
    }

    return change_evs;
}

uint32_t IoManager::DelEvent(int fd, uint32_t evs) {
    if (fd < 0 || fd >= (int)fd_ctxs_.size()) {
        return 0;
    }
    auto& ctx = fd_ctxs_[fd];
    uint32_t change_evs = ctx.ev_flags & evs;
    if (!change_evs) {
        return 0;
    }
    uint32_t remain_evs = ctx.ev_flags & ~change_evs;
    int op = remain_evs ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epev;
    memset(&epev, 0, sizeof(epev));
    epev.events = remain_evs | EPOLLET;
    epev.data.ptr = &ctx;
    if (epoll_ctl(epoll_fd_, op, fd, &epev)) {
        LOGFATAL("DelEvent" << XCO_FUNC_WITH_ARG_EXP(fd, op, evs, change_evs, remain_evs));
        assert(false);
        return 0;
    }

    if (change_evs & EPOLLIN) {
        ctx.read_co.reset();
    }
    if (change_evs & EPOLLOUT){
        ctx.write_co.reset();
    }
    ctx.ev_flags = remain_evs;

    if (op == EPOLL_CTL_DEL) {
        fds.erase(fd);
    }

    return change_evs;
}

uint32_t IoManager::CncAllEvent(int fd) {
    LOGDEBUG( "CncAllEvent, " << XCO_VARS_EXP(fd));
    return TrgEvent(fd, EPOLLIN | EPOLLOUT);
}

IoManager *IoManager::GetCurIoManager() {
    return dynamic_cast<IoManager*>(GetCurScheduler());
}

void IoManager::OnIdle() {

    const static int64_t MAX_EPOLL_TIMEOUT_MS = 2000;
    const static int MAX_EPOLL_RET_EPEV_CNT = 64;

    while(true) {
        int64_t timeout = GetNextTimerDistNow();
        timeout = timeout == -1 ? MAX_EPOLL_TIMEOUT_MS : std::min(MAX_EPOLL_TIMEOUT_MS, timeout);
        int ret = 0;
        epoll_event ret_epevs[MAX_EPOLL_RET_EPEV_CNT];
        do {
            //memset(ret_epevs, 0, sizeof(epoll_event) * MAX_EPOLL_RET_EPEV_CNT);
            for (auto fd : fds) {
                LOGDEBUG(XCO_VARS_EXP(fd));
            }
            ret = epoll_wait(epoll_fd_, ret_epevs, MAX_EPOLL_RET_EPEV_CNT, timeout);
            if (ret >= 0 || errno != EINTR){
                break;
            };
        }while(true);
        LOGDEBUG("epoll_wait, " << XCO_VARS_EXP(ret))

        // 处理定时器事件
        std::vector<std::function<void()>> cbs;
        ListExpriredCb(cbs);
        for (auto cb : cbs) {
            //cb();
            Schedule(xco::Coroutine::Create(cb));
        }

        for (int i = 0; i < ret; ++i) {
            auto& ret_epev = ret_epevs[i];
            auto ctx = (FdContext*)ret_epev.data.ptr;
            if (!ctx) {
                continue;
            }

            if (ret_epev.events & (EPOLLERR | EPOLLHUP)) {
                LOGDEBUG("epoll return err, " << XCO_VARS_EXP(ctx->fd, ret_epev.events));
                ret_epev.events = (EPOLLIN | EPOLLOUT);
            }

            TrgEvent(ctx->fd, ret_epev.events);
        }
        Coroutine::Yield();
    }
}

XCO_NAMESPAVE_END