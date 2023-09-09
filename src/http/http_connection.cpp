/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：http_session.cc
      	创 建 者：xiesenxin
      	创建日期：2021/12/30
 *================================================================*/
#include "http/http_connection.h"
#include "http/http_paser.h"

XCO_NAMESPAVE_START
namespace http {

HttpConnection::HttpConnection(Socket::Ptr socket, bool auto_close) : SocketStream(socket, auto_close) {
}

HttpConnection::Ptr HttpConnection::Create(Socket::Ptr socket, bool auto_close) {
    return std::make_shared<HttpConnection>(socket, auto_close);
}

HttpResponse::Ptr HttpConnection::RecvResponse() {
    auto http_rsp_parser = HttpResponseParser::Create();
    if (!http_rsp_parser) {
        return nullptr;
    }
    int buff_size = HttpRequestParser::GetHttpRequestMaxBodySize();
    std::vector<char> buffer(buff_size, 0);
    char* data = &buffer[0];
    size_t parse_off = 0;
    size_t data_len = 0;
    int ret = ReadHandle(data, buff_size, [&parse_off, &data_len, http_rsp_parser, data](int len) -> int{
        if (len <= 0) {
            return -1;
        }
        parse_off = 0;
        data_len = len;
        while (parse_off < (size_t)len) {
            size_t parse_len = http_rsp_parser->Parse(data, len, 0, false);
            if (parse_len <= 0 || http_rsp_parser->HasError()) {
                return -1;
            }
            parse_off += parse_len;
            if (http_rsp_parser->IsFinished()) {
                return 0;
            }
        }
        return 1;
    });
    if (ret != 0) {
        Close();
        return nullptr;
    }

    size_t body_length = http_rsp_parser->GetContentLength();
    std::string body;
    auto parser = http_rsp_parser->GetParser();
    if (parser.chunked) {

    }else {
        // 填充body
        if (body_length > 0) {
            body.resize(body_length);
            int len = std::min(data_len - parse_off, body_length);
            memcpy(&body[0], data + parse_off, len);
            body_length -= len;

            // 追加body
            if (body_length > 0) {
                if (ReadFixSize(&body[len], body_length) <= 0) {
                    Close();
                    return nullptr;
                }
            }
            http_rsp_parser->GetResponse()->SetBody(body);
        }
    }


    // 初始化
//    http_rsp_parser->GetResponse()->Init();
    auto rsp = http_rsp_parser->GetResponse();
    if (rsp) {
        auto conn_value = rsp->GetHeader("connection");
        if (strcasecmp(conn_value.c_str(), "keep-alive") == 0) {
            rsp->SetIsClose(false);
        }else {
            rsp->SetIsClose(true);
        }
    }

    return http_rsp_parser->GetResponse();
}

int HttpConnection::SendRequest(HttpRequest::Ptr req) {
    std::stringstream ss;
    auto data = req->ToString();
    return WriteFixSize(data.c_str(), data.size());
}


}//namespace http
XCO_NAMESPAVE_END