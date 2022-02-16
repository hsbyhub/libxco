/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link..
      	文件名称：http_paser.h
      	创 建 者：汉森伯逸
      	创建日期：2021/12/24
 *================================================================*/
#pragma once

#include <common.h>
#include "http/http.h"
#include "http/http11_parser.rl.h"
#include "http/httpclient_parser.rl.h"

XCO_NAMESPAVE_START
namespace http {

class HttpRequestParser {
public:
    typedef std::shared_ptr<HttpRequestParser> Ptr;

public:
    /**
     * @brief 构造函数
     */
    HttpRequestParser();

    static HttpRequestParser::Ptr Create();
public:
    /**
     * @brief 解析协议
     * @param[in, out] data 协议文本内存
     * @param[in] len 协议文本内存长度
     * @return 返回实际解析的长度,并且将已解析的数据移除
     */
    size_t Parse(const char* data, size_t len, size_t off = 0);

    /**
     * @brief 是否解析完成
     * @return 是否解析完成
     */
    int IsFinished();

    /**
     * @brief 是否有错误
     * @return 是否有错误
     */
    int HasError();

    /**
     * @brief 返回HttpRequest结构体
     */
    HttpRequest::Ptr GetRequest() const { return request_;}

    /**
     * @brief 设置错误
     * @param[in] v 错误值
     */
    void SetError(int error){ error_ = error; }

    /**
     * @brief 获取消息体长度
     */
    uint64_t GetContentLength();

    /**
     * @brief 获取http_parser结构体
     */
    const http_parser& GetParser() const { return parser_; }

public:
    /**
     * @brief 返回HttpRequest协议解析的缓存大小
     */
    static uint64_t GetHttpRequestBufferSize();

    /**
     * @brief 返回HttpRequest协议的最大消息体大小
     */
    static uint64_t GetHttpRequestMaxBodySize();

private:
    http_parser parser_;
    HttpRequest::Ptr request_;
    // excute error
    int error_;
};

class HttpResponseParser {
public:
    typedef std::shared_ptr<HttpResponseParser> Ptr;

public:
    HttpResponseParser();

    static HttpResponseParser::Ptr Create();

public:
    /**
     * @brief 解析HTTP响应协议
     * @param[in, out] data 协议数据内存
     * @param[in] len 协议数据内存大小
     * @param[in] chunck 是否在解析chunck
     * @return 返回实际解析的长度,并且移除已解析的数据
     */
    size_t Parse(const char* data, size_t len, size_t offset, bool chunck);

    /**
     * @brief 是否解析完成
     */
    int IsFinished();

    /**
     * @brief 是否有错误
     */
    int HasError();

    /**
     * @brief 返回HttpResponse
     */
    HttpResponse::Ptr GetResponse() const { return response_;}

    /**
     * @brief 设置错误码
     * @param[in] v 错误码
     */
    void SetError(int error) { error_ = error;}

    /**
     * @brief 获取消息体长度
     */
    uint64_t GetContentLength();

    /**
     * @brief 返回httpclient_parser
     */
    const httpclient_parser& GetParser() const { return parser_;}

public:
    /**
     * @brief 返回HTTP响应解析缓存大小
     */
    static uint64_t GetHttpResponseBufferSize();

    /**
     * @brief 返回HTTP响应最大消息体大小
     */
    static uint64_t GetHttpResponseMaxBodySize();

private:
    httpclient_parser parser_;
    HttpResponse::Ptr response_;
    // 1001: excute error
    int error_;
};

}//namespace http
XCO_NAMESPAVE_END
