/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�test_srv.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
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
int process_cnt = 0;
int accept_co_cnt = 0;
int client_handle_co_cnt = 0;
int real_client_handle_co_cnt = 0;
std::vector<xco::Coroutine::Ptr> accept_cos;

void OnMainInt(int) {
    XCO_LOGDEBUG("main process recv SIGINT");
    kill(0, SIGINT);
    wait(nullptr);
    exit(-1);
}

void OnChildInt(int) {
    XCO_LOGDEBUG("child process recv SIGINT");
    g_listen_sock->Close();
    exit(-1);
}

void OnHandleClient(xco::Socket::Ptr client) {
    string req;
    req.resize(4096);
    // client->SetRecvTimeOut(1000);
    while(true) {
        int ret = client->Recv(&req[0], req.size());
        XCO_LOGDEBUG("recv, " << XCO_EXP_VARS(ret));
        if (ret <= 0) {
            break;
        }
        ret = client->Send(rsp);
        XCO_LOGDEBUG("send, " << XCO_EXP_VARS(ret));
    }
    real_client_handle_co_cnt--;
    XCO_LOGDEBUG(client->ToString() << " close");
    client->Close();
}

void OnAccept() {
    g_listen_sock->SetRecvTimeOut(100);
    while(true) {
        while (real_client_handle_co_cnt >= client_handle_co_cnt) {
            usleep(5);
        }
        auto sock = g_listen_sock->Accept();
        if (sock) {
            real_client_handle_co_cnt++;
            xco::Scheduler::Schedule(xco::Coroutine::Create(std::bind(OnHandleClient, sock)));
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: test_iomanager process_count accept_fiber_count on_client_handle_fiber_count" << endl;
        exit(-1);
    }
    process_cnt = atoi(argv[1]);
    accept_co_cnt = atoi(argv[2]);
    client_handle_co_cnt= atoi(argv[3]);
    signal(SIGINT, OnMainInt);

    xco::SetLogLevel(5);
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
            for (int j = 0; j < accept_co_cnt; ++j) {
                auto co = xco::Coroutine::Create(OnAccept);
                iom.Schedule(co);
                //accept_cos.push_back(co);
            }
            iom.Start();
            g_listen_sock->Close();
        }
    }

    wait(nullptr);
    return 0;
}