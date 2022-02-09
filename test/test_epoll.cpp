/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_epoll.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/

#include "coroutine.h"
#include "concurrentqueue.h"
#include <string>
#include <queue>
#include <semaphore.h>
#include <atomic>
using namespace std;

struct Task {
    xco::Coroutine co;
    int client = -1;
};
queue<Task*> task_list;

void OnHandleTask(Task* task) {
    string req;
    req.resize(4096);
    while(true) {
        while(task->client < 0) {
            task_list.push(task);
            xco::Coroutine::Yield();
        }

    }
}

int main() {

}