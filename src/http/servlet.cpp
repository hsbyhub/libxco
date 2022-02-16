/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：http_session.cc
      	创 建 者：汉森伯逸
      	创建日期：2021/01/02
 *================================================================*/

#include <fnmatch.h>
#include "http/servlet.h"
#include "common.h"

XCO_NAMESPAVE_START
namespace http {

FunctionServlet::FunctionServlet(FunctionServlet::CallBackType cb)
    : Servlet("FunctionServlet"),
      cb_(cb){
}

int32_t FunctionServlet::Handle(http::HttpRequest::Ptr req,
                                http::HttpResponse::Ptr rsp,
                                http::HttpSession::Ptr session) {
    return cb_(req, rsp, session);
}

Servlet::Ptr HoldServletCreator::Create() const {
    return servlet_;
}

std::string HoldServletCreator::GetName() const {
    return servlet_->GetName();
}

int32_t ServletDispatch::Handle(http::HttpRequest::Ptr req,
                                http::HttpResponse::Ptr rsp,
                                http::HttpSession::Ptr session) {
    auto servlet = GetPriorityServlet(req->GetPath());
    if (!servlet) {
        return 0;
    }
    return servlet->Handle(req, rsp, session);
}

ServletDispatch::ServletDispatch()
    :   Servlet("ServletDispatch"){
    default_servlet_.reset(new NullServlet("hsby/1.0.0"));
}

Servlet::Ptr ServletDispatch::GetServlet(const std::string &uri) {
    auto servlet_creator = GetFromMap(uri_to_servlet_creator_, uri);
    if (!servlet_creator) {
        return nullptr;
    }
    return servlet_creator->Create();
}
void ServletDispatch::GetUriToServletCreator(std::unordered_map<std::string, IServletCreator::Ptr>& res_uri_to_servlet_creator) {
    CopyMap(uri_to_servlet_creator_, res_uri_to_servlet_creator);
}

void ServletDispatch::SetServlet(const std::string &uri, Servlet::Ptr servlet) {
    uri_to_servlet_creator_[uri] = std::make_shared<HoldServletCreator>(servlet);
}

void ServletDispatch::SetServlet(const std::string &uri, FunctionServlet::CallBackType cb) {
    SetServlet(uri, std::make_shared<FunctionServlet>(cb));
}

void ServletDispatch::SetServletCreator(const std::string &uri, IServletCreator::Ptr servlet_creator) {
    uri_to_servlet_creator_[uri] = servlet_creator;
}

void ServletDispatch::DelServletCreator(const std::string &uri) {
    uri_to_servlet_creator_.erase(uri);
}

Servlet::Ptr ServletDispatch::GetGlobServlet(const std::string &uri) {
    for (auto it : glob_uri_to_servlet_creator_) {
        if (!fnmatch(it.first.c_str(), uri.c_str(), 0)) {
            return it.second->Create();
        }
    }
    return nullptr;
}

void ServletDispatch::GetGlobUriToServletCreator(std::unordered_map<std::string, IServletCreator::Ptr>& res_uri_to_servlet_creator) {
    CopyMap(glob_uri_to_servlet_creator_, res_uri_to_servlet_creator);
}

void ServletDispatch::SetGlobServlet(const std::string &uri, Servlet::Ptr servlet) {
    glob_uri_to_servlet_creator_[uri] = std::make_shared<HoldServletCreator>(servlet);
}

void ServletDispatch::SetGlobServlet(const std::string &uri, FunctionServlet::CallBackType cb) {
    SetGlobServlet(uri, std::make_shared<FunctionServlet>(cb));
}

void ServletDispatch::SetGlobServletCreator(const std::string &uri, IServletCreator::Ptr servlet_creator) {
    glob_uri_to_servlet_creator_[uri] = servlet_creator;
}

void ServletDispatch::DelGlobServletCreator(const std::string &uri) {
    glob_uri_to_servlet_creator_.erase(uri);
}

Servlet::Ptr ServletDispatch::GetPriorityServlet(const std::string &uri) {
    auto servlet = GetServlet(uri);
    if (servlet) {
        return servlet;
    }
    servlet = GetGlobServlet(uri);
    if (servlet) {
        return servlet;
    }
    return default_servlet_;
}

void ServletDispatch::SetDefaultServlet(Servlet::Ptr servlet) {
    default_servlet_ = servlet;
}

Servlet::Ptr ServletDispatch::GetDefaultServlet() {
    return default_servlet_;
}

NullServlet::NullServlet(const std::string &name)
    : Servlet("NullServlet"),
    name_(name) {
    content_ ="<html><head><title>404 Not Found</title></head>"
              "<body>"
              "<center><h1>404 Not Found</h1></center>"
              "<hr>"
              "<center>" + name_ + "</center>"
              "</body>"
              "</html>";
}

int32_t NullServlet::Handle(http::HttpRequest::Ptr req,
                            http::HttpResponse::Ptr rsp,
                            http::HttpSession::Ptr session) {
    rsp->SetStatus(http::HttpStatus::NOT_FOUND);
    rsp->SetHeader("Server", "hsby/1.0.0");
    rsp->SetHeader("Conten-Type", "text/html");
    rsp->SetBody(content_);
    return 0;
}

}//namespace http
XCO_NAMESPAVE_END