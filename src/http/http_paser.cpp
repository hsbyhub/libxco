/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link..
      	文件名称：http_paser.cc
      	创 建 者：汉森伯逸
      	创建日期：2021/12/24
 *================================================================*/
#include "http/http_paser.h"
#include "common.h"

XCO_NAMESPAVE_START
namespace http {

static uint64_t s_http_request_buffer_size      = 64 * 1024;
static uint64_t s_http_request_max_body_size    = 64 * 1024 * 1024;
static uint64_t s_http_response_buffer_size     = 64 * 1024;
static uint64_t s_http_response_max_body_size   = 64 * 1024 * 1024;

void on_request_method(void *data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    HttpMethod m = CharsToHttpMethod(std::string(at, length).c_str());

    if(m == HttpMethod::METHOD_END) {
        parser->SetError(1000);
        return;
    }
    parser->GetRequest()->SetMethod(m);
}

void on_request_uri(void *data, const char *at, size_t length) {
}

void on_request_fragment(void *data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->GetRequest()->SetFragment(std::string(at, length));
}

void on_request_path(void *data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->GetRequest()->SetPath(std::string(at, length));
}

void on_request_query(void *data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->GetRequest()->SetQuery(std::string(at, length));
}

void on_request_version(void *data, const char *at, size_t length) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        parser->SetError(1001);
        return;
    }
    parser->GetRequest()->SetVersion(v);
}

void on_request_header_done(void *data, const char *at, size_t length) {
}

void on_request_http_field(void *data, const char *field, size_t flen
        ,const char *value, size_t vlen) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if(flen == 0) {
        return;
    }
    parser->GetRequest()->SetHeader(std::string(field, flen),std::string(value, vlen));
}

HttpRequestParser::HttpRequestParser()
    : request_(HttpRequest::Create())
    , error_(0){
    http_parser_init(&parser_);
    parser_.request_method  = on_request_method;
    parser_.request_uri     = on_request_uri;
    parser_.fragment        = on_request_fragment;
    parser_.request_path    = on_request_path;
    parser_.query_string    = on_request_query;
    parser_.http_version    = on_request_version;
    parser_.header_done     = on_request_header_done;
    parser_.http_field      = on_request_http_field;
    parser_.data            = this;
}

HttpRequestParser::Ptr HttpRequestParser::Create() {
    return std::make_shared<HttpRequestParser>();
}

size_t HttpRequestParser::Parse(const char *data, size_t len, size_t off) {
    return http_parser_execute(&parser_, data + off, len, 0);
}

int HttpRequestParser::IsFinished() {
    return http_parser_finish(&parser_);
}

int HttpRequestParser::HasError() {
    return error_ || http_parser_has_error(&parser_);
}

uint64_t HttpRequestParser::GetContentLength() {
    return request_->GetHeaderAs<uint64_t>("content-length", 0);
}

uint64_t HttpRequestParser::GetHttpRequestBufferSize() {
    return s_http_request_buffer_size;
}

uint64_t HttpRequestParser::GetHttpRequestMaxBodySize() {
    return s_http_request_max_body_size;
}

void on_response_reason(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    parser->GetResponse()->SetReason(std::string(at, length));
}

void on_response_status(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    HttpStatus status = (HttpStatus)(atoi(at));
    parser->GetResponse()->SetStatus(status);
}

void on_response_chunk(void *data, const char *at, size_t length) {
}

void on_response_version(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        parser->SetError(1001);
        return;
    }

    parser->GetResponse()->SetVersion(v);
}

void on_response_header_done(void *data, const char *at, size_t length) {
}

void on_response_last_chunk(void *data, const char *at, size_t length) {
}

void on_response_http_field(void *data, const char *field, size_t flen
        ,const char *value, size_t vlen) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    if(flen == 0) {
        return;
    }
    parser->GetResponse()->SetHeader(std::string(field, flen),std::string(value, vlen));
}

HttpResponseParser::HttpResponseParser()
    : response_(std::make_shared<HttpResponse>())
    , error_(0){
    httpclient_parser_init(&parser_);
    parser_.reason_phrase   = on_response_reason;
    parser_.status_code     = on_response_status;
    parser_.chunk_size      = on_response_chunk;
    parser_.http_version    = on_response_version;
    parser_.header_done     = on_response_header_done;
    parser_.last_chunk      = on_response_last_chunk;
    parser_.http_field      = on_response_http_field;
    parser_.data            = this;
}

HttpResponseParser::Ptr HttpResponseParser::Create() {
    return std::make_shared<HttpResponseParser>();
}

size_t HttpResponseParser::Parse(const char *data, size_t len, size_t offset, bool chunck) {
    if (chunck) {
        httpclient_parser_init(&parser_);
    }
    return httpclient_parser_execute(&parser_, data + offset, len, 0);
}

int HttpResponseParser::IsFinished() {
    return httpclient_parser_finish(&parser_);
}

int HttpResponseParser::HasError() {
    return error_ || httpclient_parser_has_error(&parser_);
}

uint64_t HttpResponseParser::GetContentLength() {
    return response_->GetHeaderAs<uint64_t>("content-length", 0);
}

uint64_t HttpResponseParser::GetHttpResponseBufferSize() {
    return s_http_response_buffer_size;
}

uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() {
    return s_http_response_max_body_size;
}

}//namespace http
XCO_NAMESPAVE_END
