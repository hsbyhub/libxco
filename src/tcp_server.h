/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/13
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

struct TcpServerConfig {
    typedef std::shared_ptr<TcpServerConfig> Ptr;

    std::vector<std::string> address_;
    int keep_alive = 0;
    int timeout = 1000 * 2 * 60;
    int ssl = 0;
    std::string id;
    std::string type = "http";
    std::string name;
    std::string cert_file;
    std::string key_file;
    std::string accept_worker;
    std::string io_worker;
    std::string process_worker;
    std::map<std::string, std::string> args;

    bool IsValid() const {
        return !address_.empty();
    }

    bool operator==(const TcpServerConfig oth) const {
        return address_ == oth.address_
            && keep_alive == oth.keep_alive
            && timeout == oth.timeout
            && id == oth.id
            && type == oth.type
            && name == oth.name
            && ssl == oth.ssl
            && cert_file == oth.cert_file
            && key_file == oth.key_file
            && accept_worker == oth.accept_worker
            && io_worker == oth.io_worker
            && process_worker == oth.process_worker
            && args == oth.args;
    }
};

class TcpServer : public std::enable_shared_from_this<TcpServer>,
                  Noncopyable,
                  public BaseDump{
public:
    typedef std::shared_ptr<TcpServer> Ptr;

protected:
    /**
     * @brief 构造函数 (不开放构造，防止share_from_this()访问失败)
     * @param[in] worker: socket客户端工作的协程调度器
     * @param[in] io_worder: todo
     * @param[in] accept_worker: 服务器接受连接的协程调度器
     * @return
     */
    TcpServer(const std::string& name = "none");

public:
    static TcpServer::Ptr Create(const std::string& name = "none");

    /**
     * @brief 析构函数
     */
    virtual ~TcpServer();

public:
    void Dump(std::ostream &os) const override;

public:
    virtual bool Bind(BaseAddress::Ptr address/*, bool ssl = false*/);
    virtual bool Bind(const std::vector<BaseAddress::Ptr>& addrs, std::vector<BaseAddress::Ptr>& fails/*, BOOL SSL = FALSE*/);
    virtual bool Start();
    virtual void Stop();

    virtual void ClientHandle(Socket::Ptr client);
    virtual void OnAccept(Socket::Ptr socket);

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
    TcpServerConfig conf_;
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