/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：socket.h
      	创 建 者：hsby
      	创建日期：2022/2/10
 *================================================================*/
#pragma once
#include <memory>
#include "address.h"
#include "common.h"

XCO_NAMESPAVE_START

class Socket : public std::enable_shared_from_this<Socket>,
               public Noncopyable,
               public BaseDump{
public:
    typedef std::shared_ptr<Socket> Ptr;
    typedef std::weak_ptr<Socket> WeakPtr;

    enum Type {
        kTTcp = SOCK_STREAM,
        kTUdp = SOCK_DGRAM,
    };

    enum Family {
        kFIPv4 = AF_INET,
        kFIPv6 = AF_INET6,
        kFUnix = AF_UNIX,
    };

public:
    Socket(int family, int type, int protocol = 0);
    ~Socket();

public:
    void Dump(std::ostream& os) const override;

public:
    /**
     * @brief 初始化
     * @param[in] sock:套接字句柄
     */
    bool Init(int sock = -1);

    /**
     * @brief 初始化套接字句柄
     * @param auto_create:是否自动创建
     * @return 是否成功
     */
    bool InitSystemSocket(bool auto_create = false);

    /**
     * @brief 创建Tcp套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateTCP(BaseAddress::Ptr address = nullptr);

    /**
     * @brief 创建Udp套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateUDP(BaseAddress::Ptr address = nullptr);

    /**
     * @brief 创建Tcp套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateTCP6(BaseAddress::Ptr address = nullptr);

    /**
     * @brief 创建Udp套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateUDP6(BaseAddress::Ptr address = nullptr);

    /**
     * @brief 创建UnixTCP套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateUnixTCP();

    /**
     * @brief 创建UnixUDP套接字
     * @param[in] address:地址
     * @return 套接字
     */
    static Socket::Ptr CreateUnixUDP();

    /**
     * @brief   接收连接
     * @return  套接字
     */
    Socket::Ptr Accept();

    /**
     * @brief   绑定地址
     * @return  是否成功
     */
    bool Bind(const BaseAddress::Ptr addr);

    /**
     * @brief 连接目标地址
     * @param[in]dest:目标地址
     * @param[in]time_out_ms:超时(毫秒)
     * @return 是否成功
     */
    bool Connect(const BaseAddress::Ptr dest, int64_t time_out_ms = -1);

    /**
     * @brief           监听
     * @param backlog   连接数量
     * @return          是否成功
     */
    bool Listen(int backlog = SOMAXCONN);

    /**
     * @brief   关闭连接
     */
    bool Close();

    /**
     * @brief 发送消息
     * @param[in] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Send(const std::string& buffer, int flags = 0);

    /**
     * @brief 接收消息
     * @param[out] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Send(const char* buffer, size_t len, int flags = 0);

    /**
     * @brief 发送消息
     * @param[in] iov:消息(iovec)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Send(const iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief 发送消息到目标地址
     * @param[in] to:目标地址
     * @param[in] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int SendTo(const BaseAddress::Ptr to, const char* buffer, size_t len, int flags = 0);

    /**
     * @brief 发送消息到目标地址
     * @param[in] to:目标地址
     * @param[in] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int SendTo(const BaseAddress::Ptr to, const std::string& buffer, int flags = 0);

    /**
     * @brief 发送消息到目标地址
     * @param[in] to:目标地址
     * @param[in] iov:消息(iovec)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int SendTo(const BaseAddress::Ptr to, const iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief 接收消息
     * @param[out] buffer:消息
     * @param[in] len:消息长度
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Recv(char* buffer, size_t len, int flags = 0);

    /**
     * @brief 接收消息
     * @param[out] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Recv(std::string& buffer, int flags = 0);

    /**
     * @brief 接收消息
     * @param[out] iov:消息(iovec)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int Recv(iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief 从from接收消息
     * @param[out] from:来源地址
     * @param[out] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int RecvFrom(BaseAddress::Ptr from, const char* buffer, size_t len, int flags = 0);

    /**
     * @brief 从from接收消息
     * @param[out] from:来源地址
     * @param[out] buffer:消息(string)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int RecvFrom(BaseAddress::Ptr from, std::string& buffer, int flags = 0);

    /**
     * @brief 从from接收消息
     * @param[out] from:来源地址
     * @param[out] iov:消息(iovec)
     * @param[in] flags:标志
     * @return 发送成功的字节数
     */
    int RecvFrom(BaseAddress::Ptr from, iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief 获取协议族
     */
    int GetFamily() const;

    /**
     * @brief 获取类型
     */
    int GetType() const;

    /**
     * @brief 获取协议
     */
    int GetProtocol() const;

    /**
     * @brief 是否已连接
     */
    bool IsConnected();

    /**
     * @brief 是否有效
     */
    bool IsValid();

    /**
     * @brief 获取错误码
     */
    int GetError() const;

    /**
     * @brief 获取发送超时
     */
    int64_t GetSendTimeOut() const;

    /**
     * @brief 设置发送超时
     */
    void SetSendTimeOut(int64_t time_out);

    /**
     * @brief 获取接收超时
     */
    int64_t GetRecvTimeOut() const;

    /**
     * @brief 设置接收超时
     */
    void SetRecvTimeOut(int64_t time_out);

    /**
     * @brief 获取套接字句柄
     */
    int GetSockfd() const;

    /**
     * @brief 获取本地地址 同时初始化本地地址
     */
    BaseAddress::Ptr GetLocalAddress();

    /**
     * @brief 获取远端地址 同时初始化本地地址
     */
    BaseAddress::Ptr GetRemoteAddress();

    /**
     * @brief 取消读
     */
    bool CancelRead();

    /**
     * @brief 取消写
     */
    bool CancelWrite();

    /**
     * @brief 取消接受连接
     */
    bool CancelAccept();

    /**
     * @brief 取消全部事件
     */
    bool CancelAll();

    /**
     * @brief   获取套接字选项
     * @return  是否成功
     */
    bool GetOption(int level, int option, void* result, socklen_t* len) const;

    /**
     * @brief   获取套接字选项
     * @return  是否成功
     */
    template<typename T>
    bool GetOption(int level, int option, T& result)  const{
        auto len = sizeof(T);
        return GetOption(level, option, &result, &len);
    }

    /**
     * @brief   设置套接字选项
     * @return  是否成功
     */
    bool SetOption(int level, int option, const void* result, socklen_t len);

    /**
     * @brief   设置套接字选项
     * @return  是否成功
     */
    template<typename T>
    bool SetOption(int level, int option, const T& result) {
        return SetOption(level, option, &result, sizeof(T));
    }

    static bool IsValidSockfd(int sockfd) {return sockfd != -1;}

private:
    int sockfd_     = -1;
    int family_     = 0;
    int type_       = 0;
    int protocol_   = 0;

    bool is_connected_              = false;
    BaseAddress::Ptr local_address  = nullptr;
    BaseAddress::Ptr remote_address = nullptr;
};

XCO_NAMESPAVE_END