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

using namespace std;
using namespace xco;

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";
int process_cnt = 0;
int client_handle_co_cnt = 0;
Socket::Ptr g_listen_sock = nullptr;

class SimpleHttpServer : public TcpServer {
public:
    typedef std::shared_ptr<SimpleHttpServer> Ptr;
public:
    static SimpleHttpServer::Ptr Create() {
        return std::make_shared<SimpleHttpServer>();
    }

public:
    void ClientHandle(Socket::Ptr client) override {
        if (!client) {
            return ;
        }
        std::string req;
        req.resize(4096);
        while(client->Recv(&req[0], req.size()) > 0) {
            client->Send(rsp);
        }
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

int main(int argc, char** argv) {
    if (argc != 3) {
        LOGFATAL("Usage: test_iomanager process_count max_client_cnt");
        exit(-1);
    }
    process_cnt = atoi(argv[1]);
    client_handle_co_cnt= atoi(argv[2]);
    signal(SIGINT, OnMainInt);

    //SetLogLevel(5);
    g_listen_sock = Socket::CreateTCP();
    assert(g_listen_sock);
    assert(g_listen_sock->Init());
    assert(g_listen_sock->Bind(Ipv4Address::Create("0.0.0.0", 80)));
    assert(g_listen_sock->Listen(128));

    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {
            signal(SIGINT, OnChildInt);
            IoManager iom;
            auto shs = SimpleHttpServer::Create();
            if (!shs->Init(g_listen_sock, &iom, client_handle_co_cnt)) {
                LOGFATAL("SimpleHttpServer::Init fail");
                exit(-1);
            }
            if (!shs->Start()) {
                LOGFATAL("SimpleHttpServer::Start fail");
                exit(-1);
            }
            iom.Start();
        }
    }


    wait(nullptr);
}