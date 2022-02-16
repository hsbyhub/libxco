/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_epoll.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/

#include "iomanager.h"
#include "hook.h"
#include <atomic>
#include <cstring>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/wait.h>
using namespace std;

struct Task {
    xco::Coroutine::Ptr co = nullptr;
    int client = -1;
};

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";
queue<Task*> task_list;
int g_listen_fd = -1;

void OnHandleTask(void* arg) {
    auto task = (Task*)arg;
    string req;
    req.resize(4096);
    while(true) {
        while(task->client < 0) {
            task_list.push(task);
            xco::Coroutine::Yield();
        }
        int client = task->client;
        // 开始读写
        int ret = read(client, &req[0], req.size());
        if (ret > 0) {
            ret = write(client, &rsp[0], rsp.size());
            continue;
        }
        LOGDEBUG("close, " << XCO_VARS_EXP(client));
        close(client);
        task->client = -1;
    }
}

void OnHandleAccept() {
    //timeval tv{0, 500};
    //setsockopt(g_listen_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    while(true) {
        while(task_list.empty()) {
            usleep(50);
        }
        int client = accept(g_listen_fd, nullptr, nullptr);
        if (client < 0) {
            continue;
        }
        // 拿出请求
        auto task = task_list.front();
        task_list.pop();
        task->client = client;
        xco::IoManager::Schedule(task->co);
    }
}

int CreateListenSocket(const char* addr_str, uint16_t port) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    int val = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (ret) {
        goto err;
    }

    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ret = inet_pton(AF_INET, addr_str, &addr.sin_addr.s_addr);
    if (ret != 1) {
        goto err;
    }

    ret = bind(sock, (sockaddr*)&addr, sizeof(addr));
    if (ret) {
        goto err;
    }

    ret = listen(sock, 1024);
    if (ret) {
        goto err;
    }

    return sock;

err:
    close(sock);
    return -1;
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

    // 获取监听套接字
    g_listen_fd = CreateListenSocket("0.0.0.0", 80);
    assert(g_listen_fd > 0);

    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {

            xco::IoManager iom;
            for (int j = 0; j < accept_co_cnt; ++j) {
                auto co = xco::Coroutine::Create(OnHandleAccept);
                iom.Schedule(co);
            }
            for (int j = 0; j < client_handle_co_cnt; ++j) {
                auto t = new Task;
                t->co = xco::Coroutine::Create(std::bind(OnHandleTask, t));
                t->client = -1;
                iom.Schedule(t->co);
            }
            iom.Start();

            close(g_listen_fd);
        }
    }

    wait(nullptr);
}