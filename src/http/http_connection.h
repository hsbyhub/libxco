/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�http_session.h
      	�� �� �ߣ���ɭ����
      	�������ڣ�2021/12/30
 *================================================================*/
#pragma once

#include <memory>
#include "socket_stream.h"
#include "http/http.h"

XCO_NAMESPAVE_START
namespace http {

class HttpConnection : public SocketStream{
public:
    typedef std::shared_ptr<HttpConnection> Ptr;

public:
    HttpConnection(Socket::Ptr socket, bool auto_close = false);

    static HttpConnection::Ptr Create(Socket::Ptr socket, bool auto_close = false);

public:
    HttpResponse::Ptr RecvResponse();

    int SendRequest(HttpRequest::Ptr req);

};

}//namespace http
XCO_NAMESPAVE_END