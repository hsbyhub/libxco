/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�test_scheduler.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
 *================================================================*/

#include "scheduler.h"

void fun() {
    while(true) {
        LOGDEBUG("fun before Yield");
        xco::Coroutine::Yield();
        LOGDEBUG("fun after Yield");
    }
}

int main() {
    xco::Scheduler sche;
    auto co = xco::Coroutine::Create(fun);
    sche.Schedule(co);
    sche.Start();
}