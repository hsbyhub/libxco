/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/13
 *================================================================*/

#include "tcp_server.h"

XCO_NAMESPAVE_START

static int64_t g_tcp_server_accept_timeout = 500;

TcpServer::TcpServer(const std::string& name)
    : name_(name)
    , io_manager_(nullptr)
    , accept_timeout(g_tcp_server_accept_timeout){
}

TcpServer::Ptr TcpServer::Create(const std::string &name) {
    return TcpServer::Ptr(new TcpServer(name));
}

TcpServer::~TcpServer() {
    for (const auto& it : sockets_) {
        it->Close();
    }
}

void TcpServer::Dump(std::ostream &os) const {
    if (!is_init_) {
        os << "TcpServer no init";
        return;
    }
    os << "TcpServer{" << XCO_VARS_EXP(type_, name_, ssl_, accept_timeout);
    os << "\"sockets\":[";
    for (auto& socket : sockets_) {
        os << "{" << "socket:" << *socket << "},";
    }
    os << "]}";
}

bool TcpServer::Init(BaseAddress::Ptr address, IoManager* io_manager) {
    if (is_init_) {
        return false;
    }
    if (!Bind({address})) {
        return false;
    }
    if (!io_manager) {
        return false;
    }
    io_manager_ = io_manager;
    is_init_ = true;
    return true;
}

bool TcpServer::Init(Socket::Ptr socket, IoManager *io_manager) {
    if (is_init_) {
        return false;
    }
    if (!socket || !socket->IsValid()) {
        return false;
    }
    sockets_.push_back(socket);
    if (!io_manager) {
        return false;
    }
    io_manager_ = io_manager;
    is_init_ = true;
    return true;
}

bool TcpServer::Bind(const std::vector<BaseAddress::Ptr>& addrs/*, bool ssl*/) {
    for (const auto& addr : addrs) {
        auto sock = Socket::CreateTCP(addr);
        if (!sock) {
            goto fail;
        }
        if (!sock->Init()) {
            goto fail;
        }
        if (!sock->Bind(addr)) {
            goto fail;
        }
        if (!sock->Listen(128)) {
            goto fail;
        }
        sockets_.push_back(sock);
    }
    return true;
fail:
    sockets_.clear();
    return false;
}

bool TcpServer::Start() {
    if (!is_init_) {
        return false;
    }
    if (!GetIsStop()) {
        return true;
    }
    SetIsStop(false);

    for (const auto& socket : sockets_) {
        io_manager_->Schedule(Coroutine::Create(std::bind(&TcpServer::OnAccept, shared_from_this(), socket)));
    }

    // 开始主循环
    io_manager_->Start();

    return true;
}

void TcpServer::Stop() {
    if (!is_init_) {
        return;
    }
    SetIsStop(true);
    io_manager_->Schedule(Coroutine::Create([this]() {
        for (const auto &socket: sockets_) {
            socket->Close();
        }
        sockets_.clear();
    }));
}

void TcpServer::ClientHandle(Socket::Ptr client) {
    std::string recv_buf;
    while (client->Recv(recv_buf) > 0) {
        if (recv_buf == "0000") {
            Stop();
            break;
        }
    }
}

void TcpServer::OnAccept(Socket::Ptr listen_socket) {
    if (accept_timeout != -1) {
        listen_socket->SetRecvTimeOut(g_tcp_server_accept_timeout);
    }
    while(!GetIsStop()) {
        auto client = listen_socket->Accept();
        if (!client) {
            continue;
        }
        io_manager_->Schedule(Coroutine::Create(std::bind(&TcpServer::ClientHandle, shared_from_this(), client)));
    }
}

XCO_NAMESPAVE_END