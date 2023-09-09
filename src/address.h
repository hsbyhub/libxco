/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：address.h
      	创 建 者：xiesenxin
      	创建日期：2022/2/10
 *================================================================*/
#pragma once

#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/un.h>
#include <vector>
#include <map>
#include "common.h"

XCO_NAMESPAVE_START
/**
 * @brief 地址基类
 */
class BaseAddress : public BaseDump{
public:
    typedef std::shared_ptr<BaseAddress> Ptr;

    struct InterfaceInfoUnit {
        std::string name;
        BaseAddress::Ptr address;
        uint32_t prefix;
    };

public:
    virtual ~BaseAddress() {}

    /**
     * @brief   通过判断地址类型，创建相应的地址类
     * @return   成功返回指针，失败返回nullptr
     */
    static BaseAddress::Ptr Create(const sockaddr& addr);

    /**
     * @brief               解析host(通过result返回符合条件的地址)
     * @param[out] result   地址结果
     * @param[in] host      主机名
     * @param[in] family    协议族
     * @param[in] type      协议类型
     * @param[in] protocol  协议
     * @return              是否成功
     */
    static bool LookUp(std::vector<BaseAddress::Ptr>& result, const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               解析host, 直接返回任意一个符合条件的地址
     * @param[in] host      主机名
     * @param[in] family    协议族
     * @param[in] type      协议类型
     * @param[in] protocol  协议
     * @return              地址
     */
    static BaseAddress::Ptr LookUpAny(const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               解析host, 直接返回任意一个符合条件的Ip地址
     * @param[in] host      主机名
     * @param[in] family    协议族
     * @param[in] type      协议类型
     * @param[in] protocol  协议
     * @return              地址
     */
    static BaseAddress::Ptr LookUpAnyIpAddress(const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               获取本地网卡信息
     * @param[out] result   结果
     * @param[in] family    协议族
     * @return              是否成功
     */
    static bool GetInterfaceAddresses(std::vector<InterfaceInfoUnit>& result,
                                      int family = AF_UNSPEC);

    /**
     * @brief               获取本地网卡信息
     * @param[out] result   结果
     * @param[in] iface     网卡名
     * @param[in] family    协议族
     * @return              是否成功
     */
    static bool GetInterfaceAddresses(std::vector<InterfaceInfoUnit>& result,
                                      const std::string& iface,
                                      int family = AF_INET);

    /**
     * @brief 取sockaddr 只读
     */
    virtual const sockaddr* GetAddr() const = 0;

    /**
     * @brief 取sockaddr 可写
     */
    virtual sockaddr* GetAddr() = 0;

    /**
     * @brief 取sockaddr长度
     */
    virtual socklen_t GetAddrLen() const = 0;

    /**
     * @brief 取协议族
     */
    int GetFamily() const;

    bool operator<(const BaseAddress& rhs) const;
    bool operator==(const BaseAddress& rhs) const;
    bool operator!=(const BaseAddress& rhs) const;
};

/**
 * @brief Ip地址类
 */
class IpAddress : public BaseAddress {
public:
    typedef std::shared_ptr<IpAddress> Ptr;

    /**
     * @brief               根据主机名创建地址类
     * @param[in] hostname  主机名
     * @param[in] port      端口号
     * @return              地址类指针
     */
    static IpAddress::Ptr Create(const char* hostname = nullptr, uint16_t port = 0);

    /**
     * @brief                   获取广播地址
     * @param[in] prefix_len    前缀长度
     * @return                  地址指针
     */
    virtual IpAddress::Ptr BroacastAddress(uint32_t prefix_len) = 0;

    /**
     * @brief                   获取网络地址
     * @param[in] prefix_len    前缀长度
     * @return                  地址指针
     */
    virtual IpAddress::Ptr NetworkAddress(uint32_t prefix_len) = 0;

    /**
     * @brief                   获取子网掩码
     * @param[in] prefix_len    前缀长度
     * @return                  地址指针
     */
    virtual IpAddress::Ptr SubnetMaskAddress(uint32_t prefix_len) = 0;

    /**
     * @brief 获取端口号
     */
    virtual uint16_t GetPort() const = 0;

    /**
     * @brief 设置端口号
     */
    virtual void SetPort(uint16_t port) = 0;
};

/**
 * @brief IPv4地址类
 */
class Ipv4Address : public IpAddress {
public:
    typedef std::shared_ptr<Ipv4Address> Ptr;

public:
    Ipv4Address();
    Ipv4Address(const sockaddr_in& addr);

public:
    void Dump(std::ostream &os) const override;

    /**
     * @brief                   创建地址类
     * @param[in] address_int   整型地址
     * @param[in] port          端口号
     * @return                  地址类指针
     */
    static Ipv4Address::Ptr Create(uint32_t address_int = INADDR_ANY, uint16_t port = 0);

    /**
     * @brief                   创建地址类
     * @param[in] address_str   字符串地址
     * @param[in] port          端口号
     * @return                  地址类指针
     */
    static Ipv4Address::Ptr Create(const char* address_str = nullptr, uint16_t port = 0);

    const sockaddr* GetAddr() const override;
    sockaddr* GetAddr() override;
    socklen_t GetAddrLen() const override;

    IpAddress::Ptr BroacastAddress(uint32_t prefix_len) override;
    IpAddress::Ptr NetworkAddress(uint32_t prefix_len) override;
    IpAddress::Ptr SubnetMaskAddress(uint32_t prefix_len) override;

    uint16_t GetPort() const override;
    void SetPort(uint16_t port) override;

private:
    sockaddr_in m_sockaddr;
};

/**
 * @brief IPv6地址类
 */
class Ipv6Address : public IpAddress {
public:
    typedef std::shared_ptr<Ipv6Address> Ptr;

public:
    Ipv6Address();
    Ipv6Address(const sockaddr_in6& addr);

public:
    void Dump(std::ostream &os) const override;

    /**
     * @brief               创建地址类
     * @param[in] address   地址
     * @param[in] port      端口号
     * @return              地址类指针
     */
    static Ipv6Address::Ptr Create(const char* address = nullptr, uint16_t port = 0);

    const sockaddr* GetAddr() const override;
    sockaddr* GetAddr() override;
    socklen_t GetAddrLen() const override;

    IpAddress::Ptr BroacastAddress(uint32_t prefix_len) override;
    IpAddress::Ptr NetworkAddress(uint32_t prefix_len) override;
    IpAddress::Ptr SubnetMaskAddress(uint32_t prefix_len) override;

    uint16_t GetPort() const override;
    void SetPort(uint16_t port) override;


private:
    sockaddr_in6 m_sockaddr;
};

/**
 * @brief Unix域地址类
 */
class UnixAddress : public BaseAddress {
public:
    typedef std::shared_ptr<UnixAddress> Ptr;

public:
    UnixAddress();
    UnixAddress(const std::string& path);

public:
    void Dump(std::ostream& os) const override;

    const sockaddr* GetAddr() const override;
    sockaddr* GetAddr() override;
    socklen_t GetAddrLen() const override;
    void SetAddrLen(socklen_t len);
    std::string GetPath() const;


private:
    struct sockaddr_un m_sockaddr;
    socklen_t m_length = 0;
};

class UnknownAddress : public BaseAddress {
public:
    typedef std::shared_ptr<UnknownAddress> Ptr;

public:
    UnknownAddress(int family = 0);
    UnknownAddress(const sockaddr& addr);

    const sockaddr* GetAddr() const override;
    sockaddr* GetAddr() override;
    socklen_t GetAddrLen() const override;
    void Dump(std::ostream& os) const override;

private:
    sockaddr m_sockaddr;
};
XCO_NAMESPAVE_END