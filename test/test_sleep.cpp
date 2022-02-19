/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�test_sleep.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
 *================================================================*/
#include "iomanager.h"
#include "hook.h"

void func() {
    while(true) {
        XCO_LOGDEBUG("sleep");
        usleep(100);
    }
}

int main() {
    auto co = xco::Coroutine::Create(func);
    xco::IoManager iom;
    iom.Schedule(co);
    iom.Start();
}