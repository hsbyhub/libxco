/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�http_session.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2021/12/30
 *================================================================*/
#include "http/http_session.h"
#include "http/http_paser.h"
#include "common.h"

XCO_NAMESPAVE_START
namespace http {

HttpSession::HttpSession(Socket::Ptr socket, bool auto_close) : SocketStream(socket, auto_close) {
}

HttpSession::Ptr HttpSession::Create(Socket::Ptr socket, bool auto_close) {
    return std::make_shared<HttpSession>(socket, auto_close);
}

HttpRequest::Ptr HttpSession::RecvRequest() {
    auto http_req_parser = HttpRequestParser::Create();
    if (!http_req_parser) {
        return nullptr;
    }
    int buff_size = HttpRequestParser::GetHttpRequestMaxBodySize();
    std::vector<char> buffer(buff_size, 0);
    char* data = &buffer[0];
    bool is_finish = false;
    int parse_off = 0;
    int read_off = 0;

    while(read_off < buff_size ) {
        // ��ȡ����
        int read_len = Read(data + parse_off, buff_size - parse_off);
        if (read_len <= 0) {
            Close();
            return nullptr;
        }
        read_off += read_len;

        // ����
        while (parse_off < read_off) {
            int parse_len = http_req_parser->Parse(data, read_off, parse_off);
            if (parse_len <= 0 || http_req_parser->HasError()) {
                Close();
                return nullptr;
            }
            parse_off += parse_len;
            // �Ƿ����
            if (http_req_parser->IsFinished()) {
                is_finish = true;
                break;
            }
        }
        // �Ƿ����
        if (is_finish) {
            break;
        }
    }
    if (!is_finish) {
        return nullptr;
    }

    // ���body
    int body_length = http_req_parser->GetContentLength();
    //HSBY_LOG_SYSTEM_DEBUG << HSBY_VARS_EXP(body_length, read_off, parse_off);
    if (body_length > 0) {
        std::string body;
        body.resize(body_length);
        int len = std::min(read_off - parse_off, body_length);
        memcpy(&body[0], data + parse_off, len);
        body_length -= len;

        // ׷��body
        if (body_length > 0) {
            if (ReadFixSize(&body[len], body_length) <= 0) {
                Close();
                return nullptr;
            }
        }
        http_req_parser->GetRequest()->SetBody(body);
    }

    // ��ʼ��
    http_req_parser->GetRequest()->Init();

    return http_req_parser->GetRequest();
}

int HttpSession::SendResponse(HttpResponse::Ptr rsp) {
    std::stringstream ss;
    auto data = rsp->ToString();
    return WriteFixSize(data.c_str(), data.size());
}

}//namespace http
XCO_NAMESPAVE_END