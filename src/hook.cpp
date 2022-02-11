/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：hook.cpp
      	创 建 者：hsby
      	创建日期：2022/2/10
 *================================================================*/
#include "hook.h"
#include "common/common.h"
#include <dlfcn.h>
#include <cstring>
#include <stdarg.h>
#include "iomanager.h"
#include "fdmanager.h"

XCO_NAMESPAVE_START
/**
 * @brief 当前线程是否hook
 */
static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX)       \
    XX(sleep)              \
    XX(usleep)             \
    XX(nanosleep)          \
    XX(socket)             \
    XX(connect)            \
    XX(accept)             \
    XX(read)               \
    XX(readv)              \
    XX(recv)               \
    XX(recvfrom)           \
    XX(recvmsg)            \
    XX(write)              \
    XX(writev)             \
    XX(send)               \
    XX(sendto)             \
    XX(sendmsg)            \
    XX(close)              \
    XX(fcntl)              \
    XX(ioctl)              \
    XX(getsockopt)         \
    XX(setsockopt)

void hook_init() {
    static bool is_inited = false;
    if (is_inited)
        return ;

/**
 * @brief 将被hook的函数指针都指向原本的版本
 * @details dlsym从RTLD_NEXT获取name函数指针
 */
#define XX(name) name##_f = (name##_fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

static uint64_t s_connect_timeout = 5000;
struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};
static _HookIniter s_hook_initer;

bool IsHookEnable() {
    return t_hook_enable;
}

void SetHookEnable(bool flag) {
    t_hook_enable = flag;
}
XCO_NAMESPAVE_END

struct time_info{
    int cancelled = 0;
};

/**
 * @brief todo
 */
template<typename OriginFun, typename ... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
                     uint32_t event, int timeout_so, Args&&... args) {
    // 如果没有Hook
    if (!xco::IsHookEnable()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    // 取得套接字，如果取不到说明非网络套接字
    xco::FdCtx::Ptr ctx = xco::FdManagerSgt::Instance().Get(fd);
    if (!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }

    // 如果套接字已关闭
    if (ctx->IsClosed()) {
        errno = EBADF;
        return -1;
    }

    // 如果非soket或者用户指定非阻塞
    if (!ctx->IsSocket() || ctx->IsUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t time_out = ctx->GetTimeout(timeout_so);

    // 维护一个对象标记本次任务是否已经结束
    std::shared_ptr<time_info> tinfo(new time_info);

retry:
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while(n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }

    if (n == -1 && errno == EAGAIN) {
        LOGDEBUG(XCO_VARS_EXP(fd, hook_fun_name));
        auto iow = xco::IoManager::GetCurIoManager();
        xco::Timer::Ptr timer;
        std::weak_ptr<time_info> wtinfo(tinfo);

        if (time_out != (uint64_t) - 1) {
            timer = iow->AddConditionTimer(time_out, [wtinfo, fd, iow, event]() {
                auto t = wtinfo.lock();
                if (!t || t->cancelled) {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iow->TrgEvent(fd, event);
            }, wtinfo);
        }

        int rt = iow->SetEvent(fd, event, xco::Coroutine::GetCurCoroutine());

        if (!rt) {
            if (timer) {
                timer->Cancel();
            }
            return -1;
        }else {
            LOGDEBUG("Hook Yield, "<< XCO_VARS_EXP(fd, hook_fun_name));
            xco::Coroutine::Yield();
            LOGDEBUG("Hook Yield return, "<< XCO_VARS_EXP(fd, hook_fun_name));
            if (timer) {
                timer->Cancel();
            }
            if (tinfo->cancelled) {
                errno = tinfo->cancelled;
                return -1;
            }

            goto retry;
        }
    }
    return n;
}


extern "C"  {

#define XX(name) name##_fun name##_f = nullptr;
HOOK_FUN(XX);
#undef XX


unsigned int sleep(unsigned int seconds) {
    // 如果没有hook
    if (!xco::IsHookEnable()) {
        return sleep_f(seconds);
    }

    return usleep(seconds * 1000);
}

int usleep(useconds_t usec) {
    // 如果没有hook
    if (!xco::IsHookEnable()) {
        return usleep_f(usec);
    }

    timespec req, rem;
    memset(&req, 0, sizeof(req));
    memset(&rem, 0, sizeof(rem));
    req.tv_sec = usec / 1000;
    return nanosleep(&req, &rem);
}

int nanosleep(const struct timespec *req, struct timespec *rem){
    // 如果没有hook
    if (!xco::IsHookEnable()) {
        return nanosleep_f(req, rem);
    }

    auto co = xco::Coroutine::GetCurCoroutine();
    auto iom = xco::IoManager::GetCurIoManager();

    iom->AddTimer(req->tv_sec * 1000 + req->tv_nsec / 1000000, [iom, co]() {
        iom->Schedule(co);
    });
    xco::Coroutine::Yield();
    return 0;
}

int socket(int domain, int type, int protocol) {
    // 如果没有hook
    if (!xco::IsHookEnable()) {
        return socket_f(domain, type, protocol);
    }

    int fd = socket_f(domain, type, protocol);
    if (fd == -1) {
        return fd;
    }

    // 加入套接字管理器
    xco::FdManagerSgt::Instance().Get(fd, true);

    return fd;
}

int connect_with_timeout(int sockfd, const struct sockaddr *addr,
                         socklen_t addrlen, int64_t timeout_ms) {
    if (!xco::IsHookEnable()) {
        return connect_f(sockfd, addr, addrlen);
    }
    auto ctx = xco::FdManagerSgt::Instance().Get(sockfd);
    if (!ctx || ctx->IsClosed()) {
        errno = EBADF;
        return -1;
    }
    if (!ctx->IsSocket()) {
        return connect_f(sockfd, addr, addrlen);
    }
    // 如果用户指定非阻塞
    if (ctx->IsUserNonblock()) {
        return connect_f(sockfd, addr, addrlen);
    }
    // 先尝试连接
    int n = connect_f(sockfd, addr, addrlen);
    if (n == 0) {
        return 0;
    }else if (n != -1 || errno != EINPROGRESS) {
        return n;
    }

    auto iom = xco::IoManager::GetCurIoManager();
    xco::Timer::Ptr timer;
    auto tinfo = std::make_shared<time_info>();
    std::weak_ptr<time_info> winfo(tinfo);

    // 设置超时事件，默认
    if (timeout_ms != -1) {
        timer = iom->AddConditionTimer(timeout_ms, [winfo, sockfd, iom]() {
            auto t = winfo.lock();
            if (!t || t->cancelled) {
                return;
            }
            t->cancelled = ETIMEDOUT;
            iom->SetEvent(sockfd, EPOLLOUT, xco::Coroutine::GetCurCoroutine());
        }, winfo);
    }

    int rt = iom->SetEvent(sockfd, EPOLLOUT, xco::Coroutine::GetCurCoroutine());
    if (rt) {
        xco::Coroutine::Yield();
        if (timer) {
            timer->Cancel();
        }
    } else {
        if (timer) {
            timer->Cancel();
        }
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
        return -1;
    }
    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}

int connect(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen, xco::s_connect_timeout);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int accept_fd = do_io(sockfd, accept_f, "accept", EPOLLIN, SO_RCVTIMEO, addr, addrlen);
    if (accept_fd >= 0) {
        // 加入套接字管理器
        xco::FdManagerSgt::Instance().Get(accept_fd, true);
    }
    return accept_fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", EPOLLIN, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", EPOLLIN, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", EPOLLIN, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", EPOLLIN, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", EPOLLIN, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", EPOLLOUT, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", EPOLLOUT, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags) {
    return do_io(s, send_f, "send", EPOLLOUT, SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {
    return do_io(s, sendto_f, "sendto", EPOLLOUT, SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    return do_io(s, sendmsg_f, "sendmsg", EPOLLOUT, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if (!xco::IsHookEnable()) {
        return close_f(fd);
    }
    auto ctx = xco::FdManagerSgt::Instance().Get(fd);
    // 取消关于句柄的事件
    if (ctx) {
        auto iom = xco::IoManager::GetCurIoManager();
        if (iom) {
            iom->CncAllEvent(fd);
        }
        xco::FdManagerSgt::Instance().Del(fd);
    }
    return close_f(fd);
}

int fcntl(int fd, int cmd, ...) {
    if (!xco::IsHookEnable()) {
        return fcntl_f(fd, cmd);
    }
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        // arg = (int);
        case F_SETFL: {
            auto arg = va_arg(va, int);
            va_end(va);
            auto ctx  = xco::FdManagerSgt::Instance().Get(fd);
            if (!ctx || ctx->IsClosed() || !ctx->IsSocket()) {
                return fcntl_f(fd, cmd, arg);
            }
            ctx->SetIsUserNonblock(arg & O_NONBLOCK);
            if (ctx->IsSysNonblock()) {
                arg |= O_NONBLOCK;
            }else {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
            break;
        case F_GETFL: {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            auto ctx = xco::FdManagerSgt::Instance().Get(fd);
            if (!ctx || ctx->IsClosed() || !ctx->IsSocket()) {
                return arg;
            }
            if (ctx->IsUserNonblock()) {
                return arg | O_NONBLOCK;
            }else {
                return arg & ~O_NONBLOCK;
            }
        }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ: {
            int arg = va_arg(va, int);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
            break;

            // arg = (void);
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:{
            va_end(va);
            return fcntl_f(fd, cmd);
        }
            break;

            // arg = (struct flock*)
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK: {
            auto arg = va_arg(va, struct flock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
            break;

        case F_SETOWN_EX:
        case F_GETOWN_EX: {
            auto arg = va_arg(va, struct f_owner_ex*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
            break;

        default:
            return fcntl_f(fd, cmd);
            break;
    }
    va_end(va);
}

int ioctl(int fd, unsigned long request, ...) {
    // 取出可变参数
    va_list va;
    va_start(va, request);
    auto arg = va_arg(va, void*);
    va_end(va);

    // 请求是否为非阻塞
    if (FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        auto ctx = xco::FdManagerSgt::Instance().Get(fd);
        if (!ctx || ctx->IsClosed() || !ctx->IsSocket()) {
            return ioctl_f(fd, request, arg);
        }
        ctx->SetIsUserNonblock(user_nonblock);
    }
    return ioctl_f(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!xco::IsHookEnable()) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if (level == SOL_SOCKET && optval != nullptr) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            auto ctx = xco::FdManagerSgt::Instance().Get(sockfd);
            if (ctx) {
                ctx->SetTimeout(optname, TimevalToMs(*(const timeval *) optval));
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

} // extern "C"