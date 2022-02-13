/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�address.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
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
 * @brief ��ַ����
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
     * @brief   ͨ���жϵ�ַ���ͣ�������Ӧ�ĵ�ַ��
     * @return   �ɹ�����ָ�룬ʧ�ܷ���nullptr
     */
    static BaseAddress::Ptr Create(const sockaddr& addr);

    /**
     * @brief               ����host(ͨ��result���ط��������ĵ�ַ)
     * @param[out] result   ��ַ���
     * @param[in] host      ������
     * @param[in] family    Э����
     * @param[in] type      Э������
     * @param[in] protocol  Э��
     * @return              �Ƿ�ɹ�
     */
    static bool LookUp(std::vector<BaseAddress::Ptr>& result, const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               ����host, ֱ�ӷ�������һ�����������ĵ�ַ
     * @param[in] host      ������
     * @param[in] family    Э����
     * @param[in] type      Э������
     * @param[in] protocol  Э��
     * @return              ��ַ
     */
    static BaseAddress::Ptr LookUpAny(const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               ����host, ֱ�ӷ�������һ������������Ip��ַ
     * @param[in] host      ������
     * @param[in] family    Э����
     * @param[in] type      Э������
     * @param[in] protocol  Э��
     * @return              ��ַ
     */
    static BaseAddress::Ptr LookUpAnyIpAddress(const std::string& host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief               ��ȡ����������Ϣ
     * @param[out] result   ���
     * @param[in] family    Э����
     * @return              �Ƿ�ɹ�
     */
    static bool GetInterfaceAddresses(std::vector<InterfaceInfoUnit>& result,
                                      int family = AF_UNSPEC);

    /**
     * @brief               ��ȡ����������Ϣ
     * @param[out] result   ���
     * @param[in] iface     ������
     * @param[in] family    Э����
     * @return              �Ƿ�ɹ�
     */
    static bool GetInterfaceAddresses(std::vector<InterfaceInfoUnit>& result,
                                      const std::string& iface,
                                      int family = AF_INET);

    /**
     * @brief ȡsockaddr ֻ��
     */
    virtual const sockaddr* GetAddr() const = 0;

    /**
     * @brief ȡsockaddr ��д
     */
    virtual sockaddr* GetAddr() = 0;

    /**
     * @brief ȡsockaddr����
     */
    virtual socklen_t GetAddrLen() const = 0;

    /**
     * @brief ȡЭ����
     */
    int GetFamily() const;

    bool operator<(const BaseAddress& rhs) const;
    bool operator==(const BaseAddress& rhs) const;
    bool operator!=(const BaseAddress& rhs) const;
};

/**
 * @brief Ip��ַ��
 */
class IpAddress : public BaseAddress {
public:
    typedef std::shared_ptr<IpAddress> Ptr;

    /**
     * @brief               ����������������ַ��
     * @param[in] hostname  ������
     * @param[in] port      �˿ں�
     * @return              ��ַ��ָ��
     */
    static IpAddress::Ptr Create(const char* hostname = nullptr, uint16_t port = 0);

    /**
     * @brief                   ��ȡ�㲥��ַ
     * @param[in] prefix_len    ǰ׺����
     * @return                  ��ַָ��
     */
    virtual IpAddress::Ptr BroacastAddress(uint32_t prefix_len) = 0;

    /**
     * @brief                   ��ȡ�����ַ
     * @param[in] prefix_len    ǰ׺����
     * @return                  ��ַָ��
     */
    virtual IpAddress::Ptr NetworkAddress(uint32_t prefix_len) = 0;

    /**
     * @brief                   ��ȡ��������
     * @param[in] prefix_len    ǰ׺����
     * @return                  ��ַָ��
     */
    virtual IpAddress::Ptr SubnetMaskAddress(uint32_t prefix_len) = 0;

    /**
     * @brief ��ȡ�˿ں�
     */
    virtual uint16_t GetPort() const = 0;

    /**
     * @brief ���ö˿ں�
     */
    virtual void SetPort(uint16_t port) = 0;
};

/**
 * @brief IPv4��ַ��
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
     * @brief                   ������ַ��
     * @param[in] address_int   ���͵�ַ
     * @param[in] port          �˿ں�
     * @return                  ��ַ��ָ��
     */
    static Ipv4Address::Ptr Create(uint32_t address_int = INADDR_ANY, uint16_t port = 0);

    /**
     * @brief                   ������ַ��
     * @param[in] address_str   �ַ�����ַ
     * @param[in] port          �˿ں�
     * @return                  ��ַ��ָ��
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
 * @brief IPv6��ַ��
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
     * @brief               ������ַ��
     * @param[in] address   ��ַ
     * @param[in] port      �˿ں�
     * @return              ��ַ��ָ��
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
 * @brief Unix���ַ��
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