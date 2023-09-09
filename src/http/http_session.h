/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：http_session.h
      	创 建 者：xiesenxin
      	创建日期：2021/12/30
 *================================================================*/
#pragma once

#include <memory>
#include "socket_stream.h"
#include "http/http.h"

XCO_NAMESPAVE_START
namespace http {

class HttpSession : public SocketStream {
public:
    typedef std::shared_ptr<HttpSession> Ptr;

public:
    HttpSession(Socket::Ptr socket, bool auto_close = false);

    static HttpSession::Ptr Create(Socket::Ptr socket, bool auto_close = false);

public:
    HttpRequest::Ptr RecvRequest();

    int SendResponse(HttpResponse::Ptr rsp);

};

}//namespace http
XCO_NAMESPAVE_END