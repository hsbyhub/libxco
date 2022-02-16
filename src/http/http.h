/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�socket.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2021/12/21
 *================================================================*/

#include <string>
#include <memory>
#include <map>
#include "common.h"

#pragma once

XCO_NAMESPAVE_START
namespace http {

#define HTTP_LINE_FEED "\r\n"

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
    XX(100, CONTINUE,                        Continue)                        \
    XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
    XX(102, PROCESSING,                      Processing)                      \
    XX(200, OK,                              OK)                              \
    XX(201, CREATED,                         Created)                         \
    XX(202, ACCEPTED,                        Accepted)                        \
    XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
    XX(204, NO_CONTENT,                      No Content)                      \
    XX(205, RESET_CONTENT,                   Reset Content)                   \
    XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
    XX(207, MULTI_STATUS,                    Multi-Status)                    \
    XX(208, ALREADY_REPORTED,                Already Reported)                \
    XX(226, IM_USED,                         IM Used)                         \
    XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
    XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
    XX(302, FOUND,                           Found)                           \
    XX(303, SEE_OTHER,                       See Other)                       \
    XX(304, NOT_MODIFIED,                    Not Modified)                    \
    XX(305, USE_PROXY,                       Use Proxy)                       \
    XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
    XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
    XX(400, BAD_REQUEST,                     Bad Request)                     \
    XX(401, UNAUTHORIZED,                    Unauthorized)                    \
    XX(402, PAYMENT_REQUIRED,                Payment Required)                \
    XX(403, FORBIDDEN,                       Forbidden)                       \
    XX(404, NOT_FOUND,                       Not Found)                       \
    XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
    XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
    XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
    XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
    XX(409, CONFLICT,                        Conflict)                        \
    XX(410, GONE,                            Gone)                            \
    XX(411, LENGTH_REQUIRED,                 Length Required)                 \
    XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
    XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
    XX(414, URI_TOO_LONG,                    URI Too Long)                    \
    XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
    XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
    XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
    XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
    XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
    XX(423, LOCKED,                          Locked)                          \
    XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
    XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
    XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
    XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
    XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
    XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
    XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
    XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
    XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
    XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
    XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
    XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
    XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
    XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
    XX(508, LOOP_DETECTED,                   Loop Detected)                   \
    XX(510, NOT_EXTENDED,                    Not Extended)                    \
    XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

enum class HttpStatus{
#define XX(num, name, string) name = num,
    HTTP_STATUS_MAP(XX)
#undef XX
    STATUS_END,
};

/* Request Methods */
#define HTTP_METHOD_MAP(XX)         \
    XX(0,  DELETE,      DELETE)       \
    XX(1,  GET,         GET)          \
    XX(2,  HEAD,        HEAD)         \
    XX(3,  POST,        POST)         \
    XX(4,  PUT,         PUT)          \
    /* pathological */                \
    XX(5,  CONNECT,     CONNECT)      \
    XX(6,  OPTIONS,     OPTIONS)      \
    XX(7,  TRACE,       TRACE)        \
    /* WebDAV */                      \
    XX(8,  COPY,        COPY)         \
    XX(9,  LOCK,        LOCK)         \
    XX(10, MKCOL,       MKCOL)        \
    XX(11, MOVE,        MOVE)         \
    XX(12, PROPFIND,    PROPFIND)     \
    XX(13, PROPPATCH,   PROPPATCH)    \
    XX(14, SEARCH,      SEARCH)       \
    XX(15, UNLOCK,      UNLOCK)       \
    XX(16, BIND,        BIND)         \
    XX(17, REBIND,      REBIND)       \
    XX(18, UNBIND,      UNBIND)       \
    XX(19, ACL,         ACL)          \
    /* subversion */                  \
    XX(20, REPORT,      REPORT)       \
    XX(21, MKACTIVITY,  MKACTIVITY)   \
    XX(22, CHECKOUT,    CHECKOUT)     \
    XX(23, MERGE,       MERGE)        \
    /* upnp */                        \
    XX(24, MSEARCH,     M-SEARCH)     \
    XX(25, NOTIFY,      NOTIFY)       \
    XX(26, SUBSCRIBE,   SUBSCRIBE)    \
    XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
    /* RFC-5789 */                    \
    XX(28, PATCH,       PATCH)        \
    XX(29, PURGE,       PURGE)        \
    /* CalDAV */                      \
    XX(30, MKCALENDAR,  MKCALENDAR)   \
    /* RFC-2068, section 19.6.1.2 */  \
    XX(31, LINK,        LINK)         \
    XX(32, UNLINK,      UNLINK)       \
    /* icecast */                     \
    XX(33, SOURCE,      SOURCE)       \

enum class HttpMethod{
#define XX(num, name, string) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    METHOD_END,
};

/**
 * @brief ���Դ�Сд�ȽϷº���
 */
struct CaseInsensitiveCompare {
    /**
     * @brief ���Դ�Сд�Ƚ��ַ���
     */
    bool operator()(const std::string& lhs, const std::string& rhs) const;
};

/**
 * @brief ���ַ���������ת��HTTP����ö��
 * @param[in] method_str HTTP����
 * @return HTTP����ö��
 */
HttpMethod StringToHttpMethod(const std::string& method_str);

/**
 * @brief ���ַ���ָ��ת����HTTP����ö��
 * @param[in] m �ַ�������ö��
 * @return HTTP����ö��
 */
HttpMethod CharsToHttpMethod(const char* m);

/**
 * @brief ��HTTP����ö��ת�����ַ���
 * @param[in] m HTTP����ö��
 * @return �ַ���
 */
const char* HttpMethodToString(const HttpMethod& m);

/**
 * @brief ��HTTP״̬ö��ת�����ַ���
 * @param[in] m HTTP״̬ö��
 * @return �ַ���
 */
const char* HttpStatusToString(const HttpStatus& s);


class HttpResponse;
class HttpRequest : public BaseDump{
public:
    typedef std::shared_ptr<HttpRequest> Ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveCompare> MapType;

public:
    /**
     * @brief ���캯��
     * @param[in] version �汾
     * @param[in] close �Ƿ�keepalive
     */
    HttpRequest(uint8_t version = 0x11, bool close = true);

    static std::shared_ptr<HttpRequest> Create(uint8_t version = 0x11, bool close = true);

    std::shared_ptr<HttpResponse> CreateResponse();

public:
    void Dump(std::ostream& os) const override;

public:
    void Init();
    void InitParam();
    void InitQueryParam();
    void InitBodyParam();
    void InitCookies();

private:
    //// HTTP������
    HttpMethod method_;
    //// HTTP�汾
    uint8_t version_;
    //// �Ƿ��Զ��ر�
    bool is_close_;
    //// �Ƿ�Ϊwebsocket
    bool is_web_socket_;

    uint8_t paser_paraflag_;
    //// ����·��
    std::string path_;
    //// �����ѯ
    std::string query_;
    //// ����֡
    std::string fragment_;
    //// ������Ϣ��
    std::string body_;
    //// ����ͷ��ѡ��
    MapType headers_;
    //// �������
    MapType params_;
    //// ����cooki
    MapType cookies_;

public:
    FUNCTION_BUILDER_VAR(Method, method_);
    FUNCTION_BUILDER_VAR(Version, version_);
    FUNCTION_BUILDER_VAR(IsClose, is_close_);
    FUNCTION_BUILDER_VAR(IsWebSocket, is_web_socket_);
    FUNCTION_BUILDER_VAR(Path, path_);
    FUNCTION_BUILDER_VAR(Query, query_);
    FUNCTION_BUILDER_VAR(Fragment,fragment_);
    FUNCTION_BUILDER_VAR(Body, body_);

    FUNCTION_BUILDER_MAP_AS(Header,headers_);
    FUNCTION_BUILDER_VAR(Headers, headers_);
    FUNCTION_BUILDER_MAP(Header, headers_);

    FUNCTION_BUILDER_VAR(Params, params_);
    FUNCTION_BUILDER_MAP_AS_WITH_PREDEAL(Param, params_, InitQueryParam();InitBodyParam(););
    FUNCTION_BUILDER_MAP(Param, params_);

    FUNCTION_BUILDER_VAR(Cookies, cookies_);
    FUNCTION_BUILDER_MAP_AS_WITH_PREDEAL(Cookie, cookies_, InitCookies(););
    FUNCTION_BUILDER_MAP(Cookie, cookies_);
};

class HttpResponse : public BaseDump{
public:
    typedef std::shared_ptr<HttpResponse> Ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveCompare> MapType;

public:
    HttpResponse(uint8_t version = 0x11, bool close = true);

    static HttpResponse::Ptr Create(uint8_t version = 0x11, bool close = true);

public:
    void Dump(std::ostream& os) const;

public:
    /**
     * @brief �����ض���
     * @param[in] uri:��Դ��λ��
     */
    void SetRedirect(const std::string& uri);


private:
    //// ��Ӧ״̬
    HttpStatus status_;
    //// �汾
    uint8_t version_;
    //// �Ƿ��Զ��ر�
    bool is_close_;
    //// �Ƿ�Ϊwebsocket
    bool is_websocket_;
    //// ��Ӧ��Ϣ��
    std::string body_;
    //// ��Ӧԭ��
    std::string reason_;
    //// ��Ӧͷ��MAP
    MapType headers_;
    //// cookies
    std::vector<std::string> cookies_;

public:
    FUNCTION_BUILDER_VAR(Status, status_);
    FUNCTION_BUILDER_VAR(Version, version_);
    FUNCTION_BUILDER_VAR(IsClose, is_close_);
    FUNCTION_BUILDER_VAR(IsWebScoket, is_websocket_);
    FUNCTION_BUILDER_VAR(Body, body_);
    FUNCTION_BUILDER_VAR(Reason, reason_);
    FUNCTION_BUILDER_VAR(Headers, headers_);
    FUNCTION_BUILDER_MAP(Header, headers_);
    FUNCTION_BUILDER_MAP_AS(Header, headers_);
};

}//namespace http
XCO_NAMESPAVE_END