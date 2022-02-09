/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_epoll.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/

#include "iomanager.h"
#include <string>
#include <queue>
#include <semaphore.h>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
using namespace std;

struct Task {
    xco::Coroutine* co = nullptr;
    int client = -1;
};

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:1\r\n\r\na\r\n";
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
        // 设置监听事件
        auto iom = xco::IoManager::GetCurIoManager();
        iom->SetEvent(client, EPOLLIN, task->co);
        xco::Coroutine::Yield();

        // 开始读写
        int ret = read(client, &req[0], req.size());
        if (ret > 0) {
            ret = write(client, &rsp[0], rsp.size());
            continue;
        }
        close(client);
        task->client = -1;
    }
}

void OnHandleAccept(void* arg) {
    while(true) {
        while(task_list.empty()) {
            xco::Coroutine::Yield();
        }
        int client = accept(g_listen_fd, nullptr, nullptr);
        if (client < 0) {
            continue;
        }
        // 拿出请求
        auto task = task_list.front();
        task_list.pop();
        task->client = client;
        auto iom = xco::IoManager::GetCurIoManager();
        iom->Schedule(task->co);
    }
}

int CreateListenSocket(const char* addr, uint16_t port) {
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: test_iomanager process_count accept_fiber_count on_client_handle_fiber_count" << endl;
        exit(-1);
    }
    int process_cnt = atoi(argv[1]);
    int accept_co_cnt = atoi(argv[2]);
    int client_handle_co_cnt= atoi(argv[3]);


    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {
            xco::IoManager iom;
            std::vector<xco::Coroutine*> accept_cos;
            for (int j = 0; j < accept_co_cnt; ++j) {
                // 获取监听套接字
                g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
                assert(g_listen_fd > 0);
                // 设置地址
                sockaddr_in addr;
                bzero(&addr, sizeof(addr));
                addr.sin_family = AF_INET;
                inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr.s_addr);
                addr.sin_port = htons(80);
                int rt = bind(g_listen_fd, (sockaddr*)&addr, sizeof(addr));
                assert(rt == 0);
                rt = listen(g_listen_fd, 1024);
                assert(rt == 0);
                auto co = new xco::Coroutine(OnHandleAccept);
                iom.Schedule(co);
                accept_cos.push_back(co);
            }
            for (int j = 0; j < client_handle_co_cnt; ++j) {
                auto t = new Task;
                t->co = new xco::Coroutine(OnHandleTask, (void*)t);
                t->client = -1;
            }
        }
    }
}