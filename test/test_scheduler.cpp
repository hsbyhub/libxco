/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_scheduler.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/

#include "scheduler.h"

void fun(void* arg) {
    while(true) {
        LOGDEBUG("fun before Yield");
        xco::Coroutine::Yield();
        LOGDEBUG("fun after Yield");
    }
}

int main() {
    xco::Scheduler sche;
    auto co = new xco::Coroutine(fun);
    sche.Schedule(co);
    sche.Start();
}