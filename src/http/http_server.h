/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：socket.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/01/01
 *================================================================*/
#pragma once

#include "tcp_server.h"
#include "http/servlet.h"

XCO_NAMESPAVE_START
namespace http {

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> Ptr;

protected:
    HttpServer(const std::string& name = "none", bool keepalive = false);

public:
    template<typename ...ArgsType>
    static HttpServer::Ptr Create(ArgsType ...args) {
        return std::shared_ptr<HttpServer>(new HttpServer(args...));
    }

public:
    void ClientHandle(Socket::Ptr client) override;

private:
    bool is_keep_alive;
    ServletDispatch::Ptr servlet_dispatch_;

public:
    FUNCTION_BUILDER_VAR(ServletDispatch, servlet_dispatch_);
};

}//namespace http
XCO_NAMESPAVE_END