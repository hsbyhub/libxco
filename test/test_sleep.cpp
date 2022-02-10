/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_sleep.cpp
      	创 建 者：hsby
      	创建日期：2022/2/10
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