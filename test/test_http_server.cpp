/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_http_server.cpp
      	创 建 者：hsby
      	创建日期：2022/2/16
 *================================================================*/

#include "http/http_server.h"
using namespace xco;
using namespace xco::http;

int process_cnt = 0;
int client_handle_co_cnt = 0;
xco::Socket::Ptr g_listen_sock = nullptr;

void OnMainInt(int) {
    LOGDEBUG("OnMainInt");
    if (g_listen_sock) {
        g_listen_sock->Close();
    }
    kill(0, SIGINT);
    wait(nullptr);
    exit(-1);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        exit(-1);
    }

    process_cnt = atoi(argv[1]);
    client_handle_co_cnt= atoi(argv[2]);

    g_listen_sock = Socket::CreateTCP();
    assert(g_listen_sock);
    assert(g_listen_sock->Init());
    assert(g_listen_sock->Bind(Ipv4Address::Create("0.0.0.0", 80)));
    assert(g_listen_sock->Listen(128));
    MultiProcess(process_cnt, [](){
            IoManager iom;
            auto http_server = HttpServer::Create("none", true);
            if (!http_server) {
                LOGFATAL("HttpServer::Create fail");
                exit(-1);
            }
            if (!http_server->Init(g_listen_sock, &iom, client_handle_co_cnt)) {
                LOGFATAL("HttpServer::Init fail");
                exit(-1);
            }
            if (!http_server->Start()) {
                LOGFATAL("HttpServer::Start fail");
                exit(-1);
            }
            iom.Start();
        }, OnMainInt);
    return 0;
}