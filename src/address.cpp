/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：address.cpp
      	创 建 者：xiesenxin
      	创建日期：2022/2/10
 *================================================================*/
#include "address.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <netdb.h>
#include <ifaddrs.h>


XCO_NAMESPAVE_START

BaseAddress::Ptr BaseAddress::Create(const sockaddr &addr) {
    switch(addr.sa_family) {
        case AF_INET:
            return std::make_shared<Ipv4Address>(reinterpret_cast<const sockaddr_in&>(addr));
        case AF_INET6:
            return std::make_shared<Ipv6Address>(reinterpret_cast<const sockaddr_in6&>(addr));
        default:
            return std::make_shared<UnknownAddress>(addr);
    }
    return nullptr;
}

bool BaseAddress::LookUp(std::vector<BaseAddress::Ptr> &result, const std::string &host, int family, int type,
                         int protocol) {
    if (host.empty()) {
        return false;
    }
    addrinfo ai_hints, *ai_res_head;
    MemSetZero(ai_hints);
    ai_hints.ai_family = family;
    ai_hints.ai_socktype = type;
    ai_hints.ai_protocol = protocol;

    const char* service = (const char*)memchr(host.c_str(), ':', host.size()) + 1;
    const char* node_begin = nullptr;
    size_t node_size = 0;
    if (!host.empty() && host[0] == '[') {
        if (host.size() < 2) {
            return false;
        }
        const char* node_end = (const char*) memchr(host.c_str() + 1, ']', host.size() - 1);
        if (!node_end) {
            return false;
        }
        node_begin = &host[1];
        node_size = node_end - node_begin;
    }else {
        node_begin = &host[0];
        if (service) {
            node_size = service - node_begin - 1;
        }else {
            node_size = host.size();
        }
    }
    if (!node_begin || !node_size) {
        return false;
    }
    std::string node(node_begin, node_size);
    int ret = getaddrinfo(node.c_str(), service, &ai_hints, &ai_res_head);
    if (ret) {
        return false;
    }

    // 遍历结果链表
    auto ai_res_ptr = ai_res_head;
    while(ai_res_ptr) {
        if (ai_res_ptr->ai_addr) {
            auto addr = Create(*ai_res_ptr->ai_addr);
            if (addr) {
                result.push_back(addr);
            }
        }
        ai_res_ptr = ai_res_ptr->ai_next;
    }

    freeaddrinfo(ai_res_head);

    return !result.empty();
}

BaseAddress::Ptr BaseAddress::LookUpAny(const std::string &host, int family, int type, int protocol) {
    std::vector<BaseAddress::Ptr> addrs;
    bool ret = BaseAddress::LookUp(addrs, host, family, type, protocol);
    if (!ret || addrs.empty()) {
        return nullptr;
    }
    return addrs.front();
}

BaseAddress::Ptr BaseAddress::LookUpAnyIpAddress(const std::string &host, int family, int type, int protocol) {
    std::vector<BaseAddress::Ptr> addrs;
    bool ret = BaseAddress::LookUp(addrs, host, family, type, protocol);
    if (!ret || addrs.empty()) {
        return nullptr;
    }

    for (auto addr : addrs) {
        auto tmp = std::dynamic_pointer_cast<IpAddress>(addr);
        if (tmp) {
            return tmp;
        }
    }

    return nullptr;
}

bool BaseAddress::GetInterfaceAddresses(std::vector<InterfaceInfoUnit> &result, int family) {
    struct ifaddrs *ia_res;
    if (getifaddrs(&ia_res)) {
        return false;
    }

    try {
        for (auto ia_ptr = ia_res; ia_ptr != nullptr; ia_ptr = ia_ptr->ifa_next) {
            // 检查地址族
            if (family != AF_UNSPEC && ia_ptr->ifa_addr->sa_family != family) {
                continue;
            }

            // 检查地址
            if (!ia_ptr->ifa_addr) {
                continue;
            }
            BaseAddress::Ptr addr = Create(*ia_ptr->ifa_addr);
            if (!addr) {
                continue;
            }
            uint32_t prefix_len = 0;
            switch (ia_ptr->ifa_addr->sa_family) {
                case AF_INET: {
                    prefix_len = GetOneBitCount(((sockaddr_in *) ia_ptr->ifa_netmask)->sin_addr.s_addr);
                    break;
                }
                case AF_INET6: {
                    for (int i = 0; i < 16; ++i) {
                        prefix_len += GetOneBitCount(((sockaddr_in6 *) ia_ptr->ifa_netmask)->sin6_addr.s6_addr[i]);
                    }
                    break;
                }
                default : break;
            }

            result.push_back({ia_ptr->ifa_name, addr, prefix_len});
        }
    }catch(...) {
    }

    freeifaddrs(ia_res);

    return !result.empty();
}

bool BaseAddress::GetInterfaceAddresses(std::vector<InterfaceInfoUnit> &result,
                                        const std::string &iface, int family) {
    if (iface.empty() || iface == "*") {
        if (family == AF_INET || family == AF_UNSPEC) {
            result.push_back({"", std::make_shared<Ipv4Address>(), 0u});
        }
        if (family == AF_INET6 || family == AF_UNSPEC) {
            result.push_back({"", std::make_shared<Ipv6Address>(), 0u});
        }
        return true;
    }

    std::vector<InterfaceInfoUnit> gia_res;
    if (!GetInterfaceAddresses(gia_res, family)) {
        return false;
    }

    for (const auto& it : gia_res) {
        if (it.name == iface) {
            result.push_back(it);
        }
    }
    return !result.empty();
}

int BaseAddress::GetFamily() const {
    return GetAddr()->sa_family;
}

bool BaseAddress::operator<(const BaseAddress &rhs) const {
    socklen_t min_sock_len = std::min(GetAddrLen(), rhs.GetAddrLen());
    int ptr_cmp = memcmp(GetAddr(), rhs.GetAddr(), min_sock_len);
    return ptr_cmp < 0 ? true : (ptr_cmp > 0 ? false : (GetAddrLen() < rhs.GetAddrLen()));
}

bool BaseAddress::operator==(const BaseAddress &rhs) const {
    return GetAddrLen() == rhs.GetAddrLen() && memcmp(GetAddr(), GetAddr(), GetAddrLen()) == 0;
}

bool BaseAddress::operator!=(const BaseAddress &rhs) const {
    return !(*this == rhs);
}

IpAddress::Ptr IpAddress::Create(const char *hostname, uint16_t port) {
    if (!hostname) {
        return nullptr;
    }
    addrinfo hints, *res;
    MemSetZero(hints);

    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    int ret = getaddrinfo(hostname, NULL, &hints, &res);
    if (ret) {
        return nullptr;
    }

    try {
        if (!res->ai_addr) {
            return nullptr;
        }
        auto ptr = std::dynamic_pointer_cast<IpAddress>(BaseAddress::Create(*res->ai_addr));
        if (ptr) {
            ptr->SetPort(port);
        }
        freeaddrinfo(res);
        return ptr;
    }catch (...) {
        freeaddrinfo(res);
    }
    return nullptr;
}

Ipv4Address::Ipv4Address() {
    MemSetZero(m_sockaddr);
    m_sockaddr.sin_family = AF_INET;
}

Ipv4Address::Ipv4Address(const sockaddr_in &addr) {
    memcpy(&m_sockaddr, &addr, sizeof(sockaddr_in));
}

Ipv4Address::Ptr Ipv4Address::Create(uint32_t address_int, uint16_t port) {
    auto ptr = std::make_shared<Ipv4Address>();
    ptr->m_sockaddr.sin_port = ByteSwapOnLittleEndian(port);
    ptr->m_sockaddr.sin_addr.s_addr = ByteSwapOnLittleEndian(address_int);
    return ptr;
}

Ipv4Address::Ptr Ipv4Address::Create(const char *address_str, uint16_t port) {
    auto ptr = std::make_shared<Ipv4Address>();
    if (inet_pton(AF_INET, address_str, &ptr->m_sockaddr.sin_addr.s_addr) != 1) {
        return nullptr;
    }
    ptr->m_sockaddr.sin_port = ByteSwapOnLittleEndian(port);
    return ptr;
}

void Ipv4Address::Dump(std::ostream &os) const {
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_sockaddr.sin_addr.s_addr, buf, sizeof(buf));
    os << buf << ":" << ByteSwapOnLittleEndian(m_sockaddr.sin_port);
}

const sockaddr *Ipv4Address::GetAddr() const {
    return (sockaddr*)&m_sockaddr;
}

sockaddr *Ipv4Address::GetAddr() {
    return (sockaddr*)&m_sockaddr;
}

socklen_t Ipv4Address::GetAddrLen() const {
    return sizeof(m_sockaddr);
}

IpAddress::Ptr Ipv4Address::BroacastAddress(uint32_t prefix_len) {
    uint32_t mask = CreatePreMask<uint32_t>(prefix_len);
    if (!mask) {
        return nullptr;
    }
    return Ipv4Address::Create(ByteSwapOnLittleEndian(m_sockaddr.sin_addr.s_addr) | (~mask), ByteSwapOnLittleEndian(m_sockaddr.sin_port));
}

IpAddress::Ptr Ipv4Address::NetworkAddress(uint32_t prefix_len) {
    uint32_t mask = CreatePreMask<uint32_t>(prefix_len);
    if (!mask) {
        return nullptr;
    }
    return Ipv4Address::Create(ByteSwapOnLittleEndian(m_sockaddr.sin_addr.s_addr) & mask, ByteSwapOnLittleEndian(m_sockaddr.sin_port));
}

IpAddress::Ptr Ipv4Address::SubnetMaskAddress(uint32_t prefix_len) {
    uint32_t mask = CreatePreMask<uint32_t>(prefix_len);
    if (!mask) {
        return nullptr;
    }
    return Ipv4Address::Create(~mask, ByteSwapOnLittleEndian(m_sockaddr.sin_port));
}

uint16_t Ipv4Address::GetPort() const {
    return ByteSwapOnLittleEndian(m_sockaddr.sin_port);
}

void Ipv4Address::SetPort(uint16_t port) {
    m_sockaddr.sin_port = ByteSwapOnLittleEndian(port);
}

Ipv6Address::Ipv6Address() {
    MemSetZero(m_sockaddr);
    m_sockaddr.sin6_family = AF_INET6;
}

Ipv6Address::Ipv6Address(const sockaddr_in6& addr) {
    memcpy(&m_sockaddr, &addr, sizeof(addr));
}

Ipv6Address::Ptr Ipv6Address::Create(const char *address, uint16_t port) {
    auto ptr = std::make_shared<Ipv6Address>();
    if (!inet_pton(AF_INET6, address, ptr->m_sockaddr.sin6_addr.s6_addr)) {
        return nullptr;
    }
    ptr->m_sockaddr.sin6_port = ByteSwapOnLittleEndian(port);
    return ptr;
}

const sockaddr *Ipv6Address::GetAddr() const {
    return (sockaddr*)&m_sockaddr;
}

sockaddr *Ipv6Address::GetAddr() {
    return (sockaddr*)&m_sockaddr;
}

socklen_t Ipv6Address::GetAddrLen() const {
    return sizeof(m_sockaddr);
}

void Ipv6Address::Dump(std::ostream &os) const {
    char buf[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &m_sockaddr.sin6_addr.s6_addr, buf, sizeof(buf));
    os << "[" << buf << "]:" << ByteSwapOnLittleEndian(m_sockaddr.sin6_port);
}

IpAddress::Ptr Ipv6Address::BroacastAddress(uint32_t prefix_len) {
    if (prefix_len > 128) {
        return nullptr;
    }
    sockaddr_in6 addr;
    memcpy(&addr, &m_sockaddr, sizeof(addr));
    addr.sin6_addr.s6_addr[prefix_len / 8] |= CreateSufMask<uint8_t>(prefix_len % 8);
    for (int i = prefix_len / 8; i < 16; ++i) {
        addr.sin6_addr.s6_addr[i] |= 0xff;
    }
    return std::make_shared<Ipv6Address>(addr);
}

IpAddress::Ptr Ipv6Address::NetworkAddress(uint32_t prefix_len) {
    if (prefix_len > 128) {
        return nullptr;
    }
    sockaddr_in6 addr;
    memcpy(&addr, &m_sockaddr, sizeof(addr));
    addr.sin6_addr.s6_addr[prefix_len / 8] &= ~CreateSufMask<uint8_t>(prefix_len % 8);
    for (int i = prefix_len / 8; i < 16; ++i) {
        addr.sin6_addr.s6_addr[i] &= 0;
    }
    return std::make_shared<Ipv6Address>(addr);
}

IpAddress::Ptr Ipv6Address::SubnetMaskAddress(uint32_t prefix_len) {
    if (prefix_len > 128) {
        return nullptr;
    }
    sockaddr_in6 addr;
    memcpy(&addr, &m_sockaddr, sizeof(addr));
    MemSetZero(addr.sin6_addr);
    addr.sin6_addr.s6_addr[prefix_len / 8] |= CreateSufMask<uint8_t>(prefix_len % 8);
    for (int i = prefix_len / 8; i < 16; ++i) {
        addr.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<Ipv6Address>(addr);
}

uint16_t Ipv6Address::GetPort() const {
    return ByteSwapOnLittleEndian(m_sockaddr.sin6_port);
}

void Ipv6Address::SetPort(uint16_t port) {
    m_sockaddr.sin6_port = ByteSwapOnLittleEndian(port);
}

#define MAX_PATH_LENGTH (sizeof( ((sockaddr_un*)0)->sun_path ) - 1)

UnixAddress::UnixAddress() {
    MemSetZero(m_sockaddr);
    m_sockaddr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LENGTH;
}

UnixAddress::UnixAddress(const std::string& path) {
    MemSetZero(m_sockaddr);
    m_sockaddr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + path.size() + 1;
    if (path.size() + 1 > sizeof(m_sockaddr.sun_path)) {
        throw std::logic_error("path too long");
    }
    memcpy(m_sockaddr.sun_path, path.c_str(), path.size());
}

void UnixAddress::Dump(std::ostream &os) const {
    os << m_sockaddr.sun_path;
}

const sockaddr *UnixAddress::GetAddr() const {
    return (sockaddr*)&m_sockaddr;
}

sockaddr *UnixAddress::GetAddr() {
    return (sockaddr*)&m_sockaddr;
}

socklen_t UnixAddress::GetAddrLen() const {
    return m_length;
}

void UnixAddress::SetAddrLen(socklen_t len) {
    m_length = len;
}

std::string UnixAddress::GetPath() const {
    return m_sockaddr.sun_path;
}

UnknownAddress::UnknownAddress(int family) {
    MemSetZero(m_sockaddr);
    m_sockaddr.sa_family = family;
}

UnknownAddress::UnknownAddress(const sockaddr &addr) {
    memcpy(&m_sockaddr, &addr, sizeof(sockaddr));
}

void UnknownAddress::Dump(std::ostream &os) const {
    os << "[UnknownAddress family=" <<  m_sockaddr.sa_family << "]";
}

const sockaddr *UnknownAddress::GetAddr() const {
    return &m_sockaddr;
}

sockaddr *UnknownAddress::GetAddr() {
    return &m_sockaddr;
}

socklen_t UnknownAddress::GetAddrLen() const {
    return sizeof(m_sockaddr);
}

XCO_NAMESPAVE_END