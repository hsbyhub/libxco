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

struct Client {
    Client(xco::Coroutine::CbType c, xco::Socket::Ptr s) : sock(s), co(c, this) {}
    xco::Socket::Ptr sock = nullptr;
    xco::Coroutine co;
};

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

void OnHandleClient(void* arg) {
    string req;
    req.resize(4096);
    auto client = (Client*)arg;
    while(true) {
        int ret = client->sock->Recv(&req[0], req.size());
        LOGDEBUG(XCO_VARS_EXP(ret));
        if (ret <= 0) {
            break;
        }
        client->sock->Send(rsp);
    }
    LOGDEBUG("close");
    client->sock->Close();
}

void OnAccept(void* arg) {
    std::set<Client*> clients;
    while(true) {
        LOGDEBUG("before accept");
        auto sock = g_listen_sock->Accept();
        LOGDEBUG("after accept");
        if (sock) {
            auto client = new Client(OnHandleClient, sock);
            if (client) {
                xco::Scheduler::Schedule(&client->co);
                clients.insert(client);
            }
        }
        for (auto client : clients) {
            if (client->co.state_ == xco::Coroutine::State::kStEnd) {
                LOGDEBUG(client->sock->ToString() << " end");
                delete client;
                clients.erase(client);
            }
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
            iom.Schedule(new xco::Coroutine(OnAccept));
            iom.Start();
            g_listen_sock->Close();
        }
    }

    wait(nullptr);
    return 0;
}