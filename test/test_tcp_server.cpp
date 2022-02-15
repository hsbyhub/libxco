/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_tcp_server.cpp
      	创 建 者：hsby
      	创建日期：2022/2/14
 *================================================================*/

#include <string>
#include "tcp_server.h"
#include <assert.h>
#include <sys/wait.h>

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";
int process_cnt = 0;
int accept_co_cnt = 0;
int client_handle_co_cnt = 0;
xco::Socket::Ptr g_listen_sock = nullptr;

class SimpleHttpServer : public xco::TcpServer {
public:
    typedef std::shared_ptr<SimpleHttpServer> Ptr;
public:
    static SimpleHttpServer::Ptr Create() {
        return std::make_shared<SimpleHttpServer>();
    }

public:
    void ClientHandle(xco::Socket::Ptr client) override {
        std::string req;
        req.resize(4096);
        while(client->Recv(&req[0], req.size()) > 0) {
            client->Send(rsp);
        }
        LOGDEBUG("client close, " << client->ToString());
        client->Close();
    }
};

void OnMainInt(int) {
    LOGWARN("main process recv SIGINT");
    kill(0, SIGINT);
    wait(nullptr);
    exit(-1);
}

void OnChildInt(int) {
    LOGWARN("child process recv SIGINT");
    g_listen_sock->Close();
    exit(-1);
}

void Run() {

}

int main(int argc, char** argv) {
    if (argc != 4) {
        LOGFATAL("Usage: test_iomanager process_count accept_fiber_count on_client_handle_fiber_count");
        exit(-1);
    }
    process_cnt = atoi(argv[1]);
    accept_co_cnt = atoi(argv[2]);
    client_handle_co_cnt= atoi(argv[3]);
    signal(SIGINT, OnMainInt);

    // SetLogLevel(5);
    g_listen_sock = xco::Socket::CreateTCP();
    assert(g_listen_sock);
    assert(g_listen_sock->Init());
    assert(g_listen_sock->Bind(xco::Ipv4Address::Create("0.0.0.0", 80)));
    assert(g_listen_sock->Listen(128));

    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {
            signal(SIGINT, OnChildInt);
            xco::IoManager iom;
            auto shs = SimpleHttpServer::Create();
            if (!shs->Init(g_listen_sock)) {
                LOGFATAL("SimpleHttpServer init fail");
                return -1;
            }
            if (!shs->Start()) {
                LOGFATAL("SimpleHttpServer start fail");
                return -1;
            }
        }
    }


    wait(nullptr);
}