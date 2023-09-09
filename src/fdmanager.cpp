/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：fdmanager.cpp
      	创 建 者：xiesenxin
      	创建日期：2022/2/10
 *================================================================*/
#include "fdmanager.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "hook.h"

XCO_NAMESPAVE_START

FdCtx::FdCtx(int fd)
    : m_isInit(false)
    , m_isSocket(false)
    , m_isSysNonblock(false)
    , m_isUserNonblock(false)
    , m_isClosed(false)
    , m_fd(fd)
    , m_recvTimeout(-1)
    , m_sendTimeout(-1){
    Init();
}

FdCtx::~FdCtx() {
}

bool FdCtx::Init() {
    if (m_isInit) {
        return true;
    }
    m_isInit = true;
    m_recvTimeout = -1;

    struct stat sta;
    if (fstat(m_fd, &sta) == 0) {
        m_isInit = true;
        m_isSocket = S_ISSOCK(sta.st_mode);
    }else {
        m_isInit = false;
        m_isSocket = false;
    }

    if (m_isSocket) {
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        // 如果阻塞
        if (!(flags & O_NONBLOCK)) {
            // 设置非阻塞
            int ret = fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
            XCO_ASSERT(ret == 0);
        }
        m_isSysNonblock = true;
    }else {
        m_isSysNonblock = false;
    }

    m_isUserNonblock = false;
    m_isClosed = false;

    return m_isInit;
}

void FdCtx::SetIsInit(bool mIsInit) {
    m_isInit = mIsInit;
}

bool FdCtx::IsSocket() const {
    return m_isSocket;
}

void FdCtx::SetIsSocket(bool mIsSocket) {
    m_isSocket = mIsSocket;
}

bool FdCtx::IsSysNonblock() const {
    return m_isSysNonblock;
}

void FdCtx::SetIsSysNonblock(bool mIsSysNonblock) {
    m_isSysNonblock = mIsSysNonblock;
}

bool FdCtx::IsUserNonblock() const {
    return m_isUserNonblock;
}

void FdCtx::SetIsUserNonblock(bool mIsUserNonblock) {
    m_isUserNonblock = mIsUserNonblock;
}

bool FdCtx::IsClosed() const {
    return m_isClosed;
}

void FdCtx::SetIsClosed(bool mIsClosed) {
    m_isClosed = mIsClosed;
}

int FdCtx::SetFs() const {
    return m_fd;
}

void FdCtx::SetFs(int mFs) {
    m_fd = mFs;
}

void FdCtx::SetTimeout(int type, uint64_t val) {
    if (type == SO_RCVTIMEO) {
        m_recvTimeout  = val;
    }else {
        m_sendTimeout = val;
    }
}

uint64_t FdCtx::GetTimeout(int type) {
    if (type == SO_RCVTIMEO) {
        return m_recvTimeout;
    }
    return m_sendTimeout;
}

IoManager *FdCtx::GetIomanager() const {
    return m_iomanager;
}

void FdCtx::SetIomanager(IoManager *mIomanager) {
    m_iomanager = mIomanager;
}

FdManager::FdManager() {
    m_fdctxs.resize(64);
}

FdCtx::Ptr FdManager::Get(int fd, bool auto_create) {
    if ( fd >= (int)m_fdctxs.size()) {
        if (auto_create) {
            m_fdctxs.resize(fd * 2);
        }else {
            return nullptr;
        }
    }else if (m_fdctxs[fd]){
        return m_fdctxs[fd];
    }

    m_fdctxs[fd] = std::make_shared<FdCtx>(fd);

    return m_fdctxs[fd];
}

void FdManager::Del(int fd) {
    if ((int)m_fdctxs.size() <= fd) {
        return;
    }
    m_fdctxs[fd].reset();
}

XCO_NAMESPAVE_END