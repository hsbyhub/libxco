/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_scheduler.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/

#include "scheduler.h"

void fun() {
    while(true) {
        XCO_LOGDEBUG("fun before Yield");
        xco::Coroutine::Yield();
        XCO_LOGDEBUG("fun after Yield");
    }
}

int main() {
    xco::Scheduler sche;
    auto co = xco::Coroutine::Create(fun);
    sche.Schedule(co);
    sche.Start();
}