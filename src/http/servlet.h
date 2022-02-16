/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：http_session.cc
      	创 建 者：汉森伯逸
      	创建日期：2021/01/02
 *================================================================*/
#pragma once

#include <memory>
#include <unordered_map>
#include "http/http.h"
#include "http/http_session.h"
#include "common.h"

XCO_NAMESPAVE_START
namespace http {

class Servlet {
public:
    typedef std::shared_ptr<Servlet> Ptr;

public:
    Servlet(const std::string& name) : name_(name){}

    virtual ~Servlet() {}

    virtual int32_t Handle(http::HttpRequest::Ptr req,
                           http::HttpResponse::Ptr rsp,
                           http::HttpSession::Ptr session) = 0;


private:
    std::string name_;

public:
    FUNCTION_BUILDER_VAR_GETTER(Name, name_);
};

class FunctionServlet : public Servlet {
public:
    typedef std::shared_ptr<FunctionServlet> Ptr;
    typedef std::function<int32_t(http::HttpRequest::Ptr req,
                                  http::HttpResponse::Ptr rsp,
                                  http::HttpSession::Ptr session)> CallBackType;

public:
    FunctionServlet(CallBackType cb);

    int32_t Handle(http::HttpRequest::Ptr req,
                   http::HttpResponse::Ptr rsp,
                   http::HttpSession::Ptr session) override;

private:
    CallBackType cb_;
};

class IServletCreator {
public:
    typedef std::shared_ptr<IServletCreator> Ptr;

public:
    virtual ~IServletCreator() {}

public:
    virtual Servlet::Ptr Create() const = 0;
    virtual std::string GetName() const = 0;
};

class HoldServletCreator : public IServletCreator {
public:
    typedef std::shared_ptr<HoldServletCreator> Ptr;

public:
    HoldServletCreator(Servlet::Ptr servlet) : servlet_(servlet) {}

public:
    Servlet::Ptr Create() const override;

    std::string GetName() const override;

private:
    Servlet::Ptr servlet_;
};

template<typename Type>
class ServletCreator : public IServletCreator {
public:
    ServletCreator() {}

    Servlet::Ptr Create() const override {
        return std::make_shared<Servlet>();
    }

    std::string GetName() const override {
        return GetTypeName<Type>();
    }

};

class ServletDispatch : public Servlet {
public:
    typedef std::shared_ptr<ServletDispatch> Ptr;

public:
    ServletDispatch();

    int32_t Handle(http::HttpRequest::Ptr req,
                   http::HttpResponse::Ptr rsp,
                   http::HttpSession::Ptr session) override;

    /**
     * @brief 操作精准匹配ServletCreator
     */
    Servlet::Ptr GetServlet(const std::string& uri);
    void GetUriToServletCreator(std::unordered_map<std::string, IServletCreator::Ptr>& res_uri_to_servlet_creator);
    void SetServlet(const std::string& uri, Servlet::Ptr servlet);
    void SetServlet(const std::string& uri, FunctionServlet::CallBackType cb);
    void SetServletCreator(const std::string& uri, IServletCreator::Ptr servlet_creator);
    template<typename Type>
    void SetServletCreator(const std::string& uri) {
        SetServletCreator(uri, std::make_shared<ServletCreator<Type>>());
    }
    void DelServletCreator(const std::string& uri);

    /**
     * @brief 操作模糊匹配ServletCreator
     */
    Servlet::Ptr GetGlobServlet(const std::string& uri);
    void GetGlobUriToServletCreator(std::unordered_map<std::string, IServletCreator::Ptr>& res_uri_to_servlet_creator);
    void SetGlobServlet(const std::string& uri, Servlet::Ptr servlet);
    void SetGlobServlet(const std::string& uri, FunctionServlet::CallBackType cb);
    void SetGlobServletCreator(const std::string& uri, IServletCreator::Ptr servlet_creator);
    template<typename Type>
    void SetGlobServletCreator(const std::string& uri) {
        SetGlobServletCreator(uri, std::make_shared<ServletCreator<Type>>());
    }
    void DelGlobServletCreator(const std::string& uri);

    /**
     * @brief 按照精准优先获取Servlet
     */
    Servlet::Ptr GetPriorityServlet(const std::string& uri);

    Servlet::Ptr GetDefaultServlet();
    void SetDefaultServlet(Servlet::Ptr servlet);

private:
    std::unordered_map<std::string, IServletCreator::Ptr> uri_to_servlet_creator_;
    std::unordered_map<std::string, IServletCreator::Ptr> glob_uri_to_servlet_creator_;
    Servlet::Ptr default_servlet_;
};

class NullServlet : public Servlet {
public:
    typedef std::shared_ptr<NullServlet> Ptr;
public:
    NullServlet(const std::string& name);

    int32_t Handle(http::HttpRequest::Ptr req,
                   http::HttpResponse::Ptr rsp,
                   http::HttpSession::Ptr session) override;

private:
    std::string name_;
    std::string content_;
};

}//namespace http
XCO_NAMESPAVE_END
