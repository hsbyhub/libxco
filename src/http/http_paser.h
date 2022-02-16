/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link..
      	�ļ����ƣ�http_paser.h
      	�� �� �ߣ���ɭ����
      	�������ڣ�2021/12/24
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
     * @brief ���캯��
     */
    HttpRequestParser();

    static HttpRequestParser::Ptr Create();
public:
    /**
     * @brief ����Э��
     * @param[in, out] data Э���ı��ڴ�
     * @param[in] len Э���ı��ڴ泤��
     * @return ����ʵ�ʽ����ĳ���,���ҽ��ѽ����������Ƴ�
     */
    size_t Parse(const char* data, size_t len, size_t off = 0);

    /**
     * @brief �Ƿ�������
     * @return �Ƿ�������
     */
    int IsFinished();

    /**
     * @brief �Ƿ��д���
     * @return �Ƿ��д���
     */
    int HasError();

    /**
     * @brief ����HttpRequest�ṹ��
     */
    HttpRequest::Ptr GetRequest() const { return request_;}

    /**
     * @brief ���ô���
     * @param[in] v ����ֵ
     */
    void SetError(int error){ error_ = error; }

    /**
     * @brief ��ȡ��Ϣ�峤��
     */
    uint64_t GetContentLength();

    /**
     * @brief ��ȡhttp_parser�ṹ��
     */
    const http_parser& GetParser() const { return parser_; }

public:
    /**
     * @brief ����HttpRequestЭ������Ļ����С
     */
    static uint64_t GetHttpRequestBufferSize();

    /**
     * @brief ����HttpRequestЭ��������Ϣ���С
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
     * @brief ����HTTP��ӦЭ��
     * @param[in, out] data Э�������ڴ�
     * @param[in] len Э�������ڴ��С
     * @param[in] chunck �Ƿ��ڽ���chunck
     * @return ����ʵ�ʽ����ĳ���,�����Ƴ��ѽ���������
     */
    size_t Parse(const char* data, size_t len, size_t offset, bool chunck);

    /**
     * @brief �Ƿ�������
     */
    int IsFinished();

    /**
     * @brief �Ƿ��д���
     */
    int HasError();

    /**
     * @brief ����HttpResponse
     */
    HttpResponse::Ptr GetResponse() const { return response_;}

    /**
     * @brief ���ô�����
     * @param[in] v ������
     */
    void SetError(int error) { error_ = error;}

    /**
     * @brief ��ȡ��Ϣ�峤��
     */
    uint64_t GetContentLength();

    /**
     * @brief ����httpclient_parser
     */
    const httpclient_parser& GetParser() const { return parser_;}

public:
    /**
     * @brief ����HTTP��Ӧ���������С
     */
    static uint64_t GetHttpResponseBufferSize();

    /**
     * @brief ����HTTP��Ӧ�����Ϣ���С
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
