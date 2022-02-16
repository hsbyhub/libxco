/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�socket.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
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
     * @brief ��ʼ��
     * @param[in] sock:�׽��־��
     */
    bool Init(int sock = -1);

    /**
     * @brief ��ʼ���׽��־��
     * @param auto_create:�Ƿ��Զ�����
     * @return �Ƿ�ɹ�
     */
    bool InitSystemSocket(bool auto_create = false);

    /**
     * @brief ����Tcp�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateTCP(BaseAddress::Ptr address = nullptr);

    /**
     * @brief ����Udp�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateUDP(BaseAddress::Ptr address = nullptr);

    /**
     * @brief ����Tcp�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateTCP6(BaseAddress::Ptr address = nullptr);

    /**
     * @brief ����Udp�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateUDP6(BaseAddress::Ptr address = nullptr);

    /**
     * @brief ����UnixTCP�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateUnixTCP();

    /**
     * @brief ����UnixUDP�׽���
     * @param[in] address:��ַ
     * @return �׽���
     */
    static Socket::Ptr CreateUnixUDP();

    /**
     * @brief   ��������
     * @return  �׽���
     */
    Socket::Ptr Accept();

    /**
     * @brief   �󶨵�ַ
     * @return  �Ƿ�ɹ�
     */
    bool Bind(const BaseAddress::Ptr addr);

    /**
     * @brief ����Ŀ���ַ
     * @param[in]dest:Ŀ���ַ
     * @param[in]time_out_ms:��ʱ(����)
     * @return �Ƿ�ɹ�
     */
    bool Connect(const BaseAddress::Ptr dest, int64_t time_out_ms = -1);

    /**
     * @brief           ����
     * @param backlog   ��������
     * @return          �Ƿ�ɹ�
     */
    bool Listen(int backlog = SOMAXCONN);

    /**
     * @brief   �ر�����
     */
    bool Close();

    /**
     * @brief ������Ϣ
     * @param[in] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Send(const std::string& buffer, int flags = 0);

    /**
     * @brief ������Ϣ
     * @param[out] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Send(const char* buffer, size_t len, int flags = 0);

    /**
     * @brief ������Ϣ
     * @param[in] iov:��Ϣ(iovec)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Send(const iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief ������Ϣ��Ŀ���ַ
     * @param[in] to:Ŀ���ַ
     * @param[in] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int SendTo(const BaseAddress::Ptr to, const char* buffer, size_t len, int flags = 0);

    /**
     * @brief ������Ϣ��Ŀ���ַ
     * @param[in] to:Ŀ���ַ
     * @param[in] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int SendTo(const BaseAddress::Ptr to, const std::string& buffer, int flags = 0);

    /**
     * @brief ������Ϣ��Ŀ���ַ
     * @param[in] to:Ŀ���ַ
     * @param[in] iov:��Ϣ(iovec)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int SendTo(const BaseAddress::Ptr to, const iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief ������Ϣ
     * @param[out] buffer:��Ϣ
     * @param[in] len:��Ϣ����
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Recv(char* buffer, size_t len, int flags = 0);

    /**
     * @brief ������Ϣ
     * @param[out] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Recv(std::string& buffer, int flags = 0);

    /**
     * @brief ������Ϣ
     * @param[out] iov:��Ϣ(iovec)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int Recv(iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief ��from������Ϣ
     * @param[out] from:��Դ��ַ
     * @param[out] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int RecvFrom(BaseAddress::Ptr from, const char* buffer, size_t len, int flags = 0);

    /**
     * @brief ��from������Ϣ
     * @param[out] from:��Դ��ַ
     * @param[out] buffer:��Ϣ(string)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int RecvFrom(BaseAddress::Ptr from, std::string& buffer, int flags = 0);

    /**
     * @brief ��from������Ϣ
     * @param[out] from:��Դ��ַ
     * @param[out] iov:��Ϣ(iovec)
     * @param[in] flags:��־
     * @return ���ͳɹ����ֽ���
     */
    int RecvFrom(BaseAddress::Ptr from, iovec* iov, size_t iov_len, int flags = 0);

    /**
     * @brief ��ȡЭ����
     */
    int GetFamily() const;

    /**
     * @brief ��ȡ����
     */
    int GetType() const;

    /**
     * @brief ��ȡЭ��
     */
    int GetProtocol() const;

    /**
     * @brief �Ƿ�������
     */
    bool IsConnected();

    /**
     * @brief �Ƿ���Ч
     */
    bool IsValid();

    /**
     * @brief ��ȡ������
     */
    int GetError() const;

    /**
     * @brief ��ȡ���ͳ�ʱ
     */
    int64_t GetSendTimeOut() const;

    /**
     * @brief ���÷��ͳ�ʱ
     */
    void SetSendTimeOut(int64_t time_out);

    /**
     * @brief ��ȡ���ճ�ʱ
     */
    int64_t GetRecvTimeOut() const;

    /**
     * @brief ���ý��ճ�ʱ
     */
    void SetRecvTimeOut(int64_t time_out);

    /**
     * @brief ��ȡ�׽��־��
     */
    int GetSockfd() const;

    /**
     * @brief ��ȡ���ص�ַ ͬʱ��ʼ�����ص�ַ
     */
    BaseAddress::Ptr GetLocalAddress();

    /**
     * @brief ��ȡԶ�˵�ַ ͬʱ��ʼ�����ص�ַ
     */
    BaseAddress::Ptr GetRemoteAddress();

    /**
     * @brief ȡ����
     */
    bool CancelRead();

    /**
     * @brief ȡ��д
     */
    bool CancelWrite();

    /**
     * @brief ȡ����������
     */
    bool CancelAccept();

    /**
     * @brief ȡ��ȫ���¼�
     */
    bool CancelAll();

    /**
     * @brief   ��ȡ�׽���ѡ��
     * @return  �Ƿ�ɹ�
     */
    bool GetOption(int level, int option, void* result, socklen_t* len) const;

    /**
     * @brief   ��ȡ�׽���ѡ��
     * @return  �Ƿ�ɹ�
     */
    template<typename T>
    bool GetOption(int level, int option, T& result)  const{
        auto len = sizeof(T);
        return GetOption(level, option, &result, &len);
    }

    /**
     * @brief   �����׽���ѡ��
     * @return  �Ƿ�ɹ�
     */
    bool SetOption(int level, int option, const void* result, socklen_t len);

    /**
     * @brief   �����׽���ѡ��
     * @return  �Ƿ�ɹ�
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