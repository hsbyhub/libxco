/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�socket.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/01/01
 *================================================================*/

#include "http/http_server.h"
#include "http/http_session.h"

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
            break;
        }
        bool close = !is_keep_alive/* || req->GetIsClose()*/;
        auto rsp = HttpResponse::Create(req->GetVersion(), close);
        rsp->SetBody("some text");
        session->SendResponse(rsp);
        if (close) {
            break;
        }
        usleep(50);
    }
    session->Close();
}

}//namespace http
XCO_NAMESPAVE_END