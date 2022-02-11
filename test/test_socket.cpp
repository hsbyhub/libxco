/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_socket.cpp
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

struct Task {
    xco::Coroutine::Ptr co = nullptr;
    xco::Socket::Ptr client = nullptr;
};

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";
queue<Task*> task_list;
xco::Socket::Ptr g_listen_sock = nullptr;

void OnChildInt(int) {
    LOGDEBUG("child process recv SIGINT");
    g_listen_sock->Close();
    exit(-1);
}

void OnMainInt(int) {
    LOGDEBUG("main process recv SIGINT");
    kill(0, SIGINT);
    wait(nullptr);
    exit(-1);
}

void OnHandleTask(void* arg) {
    auto task = (Task*)arg;
    string req;
    req.resize(4096);
    while(true) {
        while(!task->client) {
            task_list.push(task);
            xco::Coroutine::Yield();
        }
        // 开始读写
        auto client = task->client;
        int ret = client->Recv(&req[0], req.size());
        if (ret > 0) {
            ret = client->Send(rsp);
            continue;
        }
        client->Close();
        task->client = nullptr;
    }
}

void OnHandleAccept(void* arg) {
    while(true) {
        while(task_list.empty()) {
            usleep(50);
        }
        auto client = g_listen_sock->Accept();
        if (!client) {
            continue;
        }
        // 拿出请求
        auto task = task_list.front();
        task_list.pop();
        task->client = client;
        xco::IoManager::Schedule(task->co);
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: test_iomanager process_count accept_fiber_count on_client_handle_fiber_count" << endl;
        exit(-1);
    }
    int process_cnt = atoi(argv[1]);
    int accept_co_cnt = atoi(argv[2]);
    int client_handle_co_cnt= atoi(argv[3]);

    SetLogLevel(4);

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
            for (int j = 0; j < accept_co_cnt; ++j) {
                auto co = xco::Coroutine::Create(OnHandleAccept);
                iom.Schedule(co);
            }
            for (int j = 0; j < client_handle_co_cnt; ++j) {
                auto t = new Task;
                t->co = xco::Coroutine::Create(OnHandleTask, (void*)t);
                iom.Schedule(t->co);
            }
            iom.Start();
            g_listen_sock->Close();
        }
    }

    wait(nullptr);
}
