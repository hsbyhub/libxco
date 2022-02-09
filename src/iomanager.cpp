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

uint32_t IoManager::SetEvent(int fd, uint32_t ev, Coroutine *co) {
    assert(fd > 0 && co);
    assert(ev && (!((ev & EPOLLIN) && (ev & EPOLLOUT))));
    if (fd >= (int)fd_ctxs_.size()) {
        fd_ctxs_.resize(fd * 2);
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
    ctx.ev_flags |= ev;
    if (ev & EPOLLIN) {
        ctx.read_co = co;
    }else if (ev & EPOLLOUT){
        ctx.write_co = co;
    }else {
        return 0;
    }
    return ev;
}

uint32_t IoManager::TrgEvent(int fd, uint32_t evs) {
    assert(fd < (int)fd_ctxs_.size());
    evs = DelEvent(fd, evs);
    auto& ctx = fd_ctxs_[fd];
    if (evs & EPOLLIN) {
        Schedule(ctx.read_co);
        ctx.read_co = nullptr;
    }else if (evs & EPOLLOUT){
        Schedule(ctx.write_co);
        ctx.write_co = nullptr;
    }
    return evs;
}

uint32_t IoManager::DelEvent(int fd, uint32_t evs) {
    assert(fd < (int)fd_ctxs_.size());
    auto& ctx = fd_ctxs_[fd];
    evs &= EPOLLIN | EPOLLOUT;
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

void IoManager::OnIdle() {
}

XCO_NAMESPAVE_END