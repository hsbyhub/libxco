/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：socket.cc
      	创 建 者：汉森伯逸
      	创建日期：2021/12/21
 *================================================================*/

#include "http/http.h"

XCO_NAMESPAVE_START
namespace http {


bool CaseInsensitiveCompare::operator()(const std::string &lhs, const std::string &rhs) const {
    if (!lhs.c_str()) {
        return true;
    }
    if (!rhs.c_str()) {
        return false;
    }
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HttpMethod StringToHttpMethod(const std::string &method_str) {
#define XX(num, name, string)                       \
    if (strcmp(#string, method_str.c_str()) == 0){  \
        return HttpMethod::name;                    \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::METHOD_END;
}

static const char* g_method_string[] =  {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

HttpMethod CharsToHttpMethod(const char *m) {
#define XX(num, name, string)                       \
    if (strcmp(#string, m) == 0) { \
        return HttpMethod::name;                    \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::METHOD_END;
}

const char *HttpMethodToString(const HttpMethod &m) {
    uint32_t idx = (uint32_t)m;
    if (sizeof(g_method_string) == 0 || idx >= (sizeof(g_method_string) / sizeof(g_method_string[0]))) {
        return "<unknown>";
    }
    return g_method_string[idx];
}

const char *HttpStatusToString(const HttpStatus &s) {
    switch (s){
#define XX(code, name, msg)     \
        case HttpStatus::name:  \
            return #msg;

        HTTP_STATUS_MAP(XX);
#undef XX
        default: return "<unknown>";
    }
}

HttpRequest::HttpRequest(uint8_t version, bool close)
    : method_(HttpMethod::GET)
    , version_(version)
    , is_close_(close)
    , is_web_socket_(false)
    , paser_paraflag_(0)
    , path_("/"){
}

std::shared_ptr<HttpRequest> HttpRequest::Create(uint8_t version, bool close) {
    return std::make_shared<HttpRequest>(version, close);
}

std::shared_ptr<HttpResponse> HttpRequest::CreateResponse() {
    return std::make_shared<HttpResponse>(GetVersion(), GetIsClose());
}

void HttpRequest::Init() {
    std::string conn = GetHeader("connection");
    if (!conn.empty()) {
        if (strcasecmp(conn.c_str(), "keep-alive") == 0) {
            SetIsClose(false);
        }else {
            SetIsClose(true);
        };
    }
}

void HttpRequest::InitParam() {
    InitBodyParam();
    InitQueryParam();
    InitCookies();
}

void HttpRequest::InitQueryParam() {

}

void HttpRequest::InitBodyParam() {

}

void HttpRequest::InitCookies() {

}

void HttpRequest::Dump(std::ostream &os) const {
    // reqs line
    os  << HttpMethodToString(method_) << " "
        << path_
        << (query_.empty() ? "" : "?")
        << query_
        << (fragment_.empty() ? "" : "#")
        << fragment_
        << " HTTP/"
        << ((int)version_ >> 4)
        << "."
        << ((int)version_ & 0x0f)
        << HTTP_LINE_FEED;

    // header
    for (auto it : headers_) {
        if (strcasecmp(it.first.c_str(), "connection") == 0) {
            continue;
        }
        if (strcasecmp(it.first.c_str(), "content-length") == 0) {
            continue;
        }
        os << it.first << ": " << it.second << HTTP_LINE_FEED;
    }
    if (!GetIsWebSocket()) {
        os << "Connection: " << (GetIsClose() ? "close" : "keep-alive") << HTTP_LINE_FEED;
    }
    if (!body_.empty()) {
        os << "Content-length: " << body_.size() << HTTP_LINE_FEED;
    }
    os << HTTP_LINE_FEED;

    // body
    os << body_;
}

HttpResponse::HttpResponse(uint8_t version, bool close)
    : status_(HttpStatus::OK)
    , version_(version)
    , is_close_(close)
    , is_websocket_(false){
}

HttpResponse::Ptr HttpResponse::Create(uint8_t version, bool close) {
    return std::make_shared<HttpResponse>(version, close);
}

void HttpResponse::Dump(std::ostream &os) const {
    // response line
    os  << "HTTP/" << (int)(version_ >> 4) << "." << (int)(version_ & 0x0f) << " "
        << (int)status_ << " "
        << (reason_.empty() ? HttpStatusToString(status_) : reason_)
        << HTTP_LINE_FEED;

    // header
    for (auto it : headers_) {
        if (!GetIsWebScoket() && strcasecmp(it.first.c_str(), "connection") == 0) {
            continue;
        }
        if (strcasecmp(it.first.c_str(), "content-length") == 0) {
            continue;
        }
        os << it.first << ": " << it.second << HTTP_LINE_FEED;
    }
    for (const auto& it : cookies_) {
        os << "Set-cookie: " << it << HTTP_LINE_FEED;
    }
    if (!GetIsWebScoket()) {
        os << "Connection: " << (GetIsClose() ? "close" : "keep-alive" ) << HTTP_LINE_FEED;
    }
    if (!body_.empty()) {
        os << "Content-length: " << body_.size() << HTTP_LINE_FEED;
    }
    os << HTTP_LINE_FEED;

    // body
    os << body_ << HTTP_LINE_FEED;
}

void HttpResponse::SetRedirect(const std::string &uri) {
    status_ = HttpStatus::FOUND;
    SetHeader("Location", uri);
}

}//namespace http
XCO_NAMESPAVE_END