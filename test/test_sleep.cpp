/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�test_sleep.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
 *================================================================*/
#include "iomanager.h"
#include "hook.h"

void func(void* arg) {
    while(true) {
        sleep(1);
    }
}

int main() {
    xco::Coroutine co(func);
    xco::IoManager iom;
    iom.Schedule(&co);
    iom.Start();
}