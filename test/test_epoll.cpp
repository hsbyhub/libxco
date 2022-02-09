/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�test_epoll.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
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