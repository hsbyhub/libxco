/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�echo_server.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/2/13
 *================================================================*/
#pragma once

#include <memory>
#include <vector>
#include <map>
#include "iomanager.h"
#include "socket.h"
#include "address.h"
#include "common.h"

XCO_NAMESPAVE_START

class TcpServer : public std::enable_shared_from_this<TcpServer>,
                  Noncopyable,
                  public BaseDump{
public:
    typedef std::shared_ptr<TcpServer> Ptr;

protected:
    /**
     * @brief ���캯�� (�����Ź��죬��ֹshare_from_this()����ʧ��)
     * @param[in] worker: socket�ͻ��˹�����Э�̵�����
     * @param[in] io_worder: todo
     * @param[in] accept_worker: �������������ӵ�Э�̵�����
     * @return
     */
    TcpServer(const std::string& name = "none");

public:
    static TcpServer::Ptr Create(const std::string& name = "none");

    /**
     * @brief ��������
     */
    virtual ~TcpServer();

public:
    void Dump(std::ostream &os) const override;

public:
    virtual bool Init(BaseAddress::Ptr address/*, bool ssl = false*/);
    virtual bool Bind(const std::vector<BaseAddress::Ptr>& addrs/*, BOOL SSL = FALSE*/);
    virtual bool Start();
    virtual void Stop();

    virtual void ClientHandle(Socket::Ptr client);
    virtual void OnAccept(Socket::Ptr listen_socket);

public:
    size_t GetSocketNum(){ return sockets_.size(); }

private:
    std::string name_;
    std::vector<Socket::Ptr> sockets_;
    IoManager io_manager_;
    int64_t read_timeout_;
    std::string type_ = "tcp";
    bool is_stop_;
    bool ssl_;
    bool is_init_ = false;

public:
    FUNCTION_BUILDER_VAR(ReadTimeout, read_timeout_);
    FUNCTION_BUILDER_VAR(Name, name_);
    FUNCTION_BUILDER_VAR(IsInit, is_init_);
    FUNCTION_BUILDER_VAR_GETTER(IsStop, is_stop_);

protected:
    FUNCTION_BUILDER_VAR_SETTER(IsStop, is_stop_);

};
XCO_NAMESPAVE_END