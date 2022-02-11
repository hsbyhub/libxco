/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_srv.cpp
      	创 建 者：hsby
      	创建日期：2022/2/10
 *================================================================*/
#include "iomanager.h"
#include "hook.h"
#include "socket.h"
#include <atomic>
#include <assert.h>
#include <sys/wait.h>
using namespace std;

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";
xco::Socket::Ptr g_listen_sock = nullptr;

void OnMainInt(int) {
    LOGDEBUG("main process recv SIGINT");
    kill(0, SIGINT);
    wait(nullptr);
    exit(-1);
}

void OnChildInt(int) {
    LOGDEBUG("child process recv SIGINT");
    g_listen_sock->Close();
    exit(-1);
}

void OnHandleClient(xco::Socket::Ptr client) {
    string req;
    req.resize(4096);
    while(true) {
        int ret = client->Recv(&req[0], req.size());
        if (ret <= 0) {
            break;
        }
        client->Send(rsp);
    }
    LOGDEBUG("close");
    client->Close();
}

void OnAccept() {
    while(true) {
        LOGDEBUG("before accept");
        auto sock = g_listen_sock->Accept();
        LOGDEBUG("after accept");
        if (sock) {
            xco::Scheduler::Schedule(xco::Coroutine::Create(std::bind(OnHandleClient, sock)));
        }
    }
}

int main(int argc, char** argv) {
    //if (argc != 4) {
    //    cout << "Usage: test_iomanager process_count accept_fiber_count on_client_handle_fiber_count" << endl;
    //    exit(-1);
    //}
    int process_cnt = atoi(argv[1]);
    //int accept_co_cnt = atoi(argv[2]);
    //int client_handle_co_cnt= atoi(argv[3]);

    SetLogLevel(5);

    g_listen_sock = xco::Socket::CreateTCP();
    assert(g_listen_sock);
    assert(g_listen_sock->Init());
    assert(g_listen_sock->Bind(xco::Ipv4Address::Create("0.0.0.0", 80)));
    assert(g_listen_sock->Listen(128));

    signal(SIGINT, OnMainInt);

    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {
            signal(SIGINT, OnChildInt);
            xco::IoManager iom;
            iom.Schedule(xco::Coroutine::Create(OnAccept));
            iom.Start();
            g_listen_sock->Close();
        }
    }

    wait(nullptr);
    return 0;
}