/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link..
      	文件名称 http_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2021/12/24
 *================================================================*/

#include "http/http_server.h"
#include "http/http_session.h"

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";

XCO_NAMESPAVE_START
namespace http {

HttpServer::HttpServer(const std::string &name, bool keepalive)
            : TcpServer(name),
              is_keep_alive(keepalive),
              servlet_dispatch_(std::make_shared<ServletDispatch>()){
}

void HttpServer::ClientHandle(Socket::Ptr client) {
    if (!client) {
        return;
    }
    auto session = HttpSession::Create(client);
    while(session->IsConnected()) {
        auto req = session->RecvRequest();
        if (!req) {
            LOGDEBUG("Recv req fail");
            break;
        }
        bool close = !is_keep_alive/* || req->GetIsClose()*/;
        auto rsp = HttpResponse::Create(req->GetVersion(), close);
        rsp->SetBody("some text");
        session->SendResponse(rsp);
        if (close) {
            break;
        }

    }
}

}//namespace http
XCO_NAMESPAVE_END