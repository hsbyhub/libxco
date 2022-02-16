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

    struct Task {
        typedef std::shared_ptr<Task> Ptr;
        Task(Coroutine::Ptr c, Socket::Ptr& cl) : co(c), cli(cl){}
        Coroutine::Ptr      co  = nullptr;
        Socket::Ptr&        cli;
    };

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
    virtual bool Init(BaseAddress::Ptr address, IoManager* io_manager = IoManager::GetCurIoManager(), uint32_t client_handler_cnt = 1024 * 10/*, bool ssl = false*/);
    virtual bool Init(Socket::Ptr socket, IoManager* io_manager = IoManager::GetCurIoManager(), uint32_t client_handler_cnt = 1024 * 10/*, bool ssl = false*/);
    virtual bool Bind(const std::vector<BaseAddress::Ptr>& addrs/*, BOOL SSL = FALSE*/);
    virtual bool Start();
    virtual void Stop();

    virtual void ClientHandle(Socket::Ptr client);
    virtual void OnAccept(Socket::Ptr listen_socket);

public:
    size_t GetSocketNum(){ return sockets_.size(); }

private:
    void OnClientHandle(Socket::Ptr client);

private:
    std::string                 name_                   = "none";
    IoManager*                  io_manager_             = nullptr;
    int64_t                     accept_timeout_         = -1;
    uint32_t                    client_handler_cnt_     = 1024 * 10;
    std::string                 type_                   = "tcp";
    bool                        is_stop_                = true;
    bool                        ssl_                    = false;
    bool                        is_init_                = false;
    std::queue<Task::Ptr>       idle_cos_;
    std::vector<Socket::Ptr>    sockets_;

public:
    FUNCTION_BUILDER_VAR(IsStop, is_stop_);
};
XCO_NAMESPAVE_END