/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�echo_server.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/2/7
 *================================================================*/

#include "coroutine.h"
#include <iostream>
#include <unistd.h>

void fun() {
    while(true) {
        //LOGDEBUG("before Yield fun_co");
        xco::Coroutine::Yield();
        //LOGDEBUG("after Yield fun_co");
    }
}

void test_resume() {
    auto co = xco::Coroutine::Create(fun);
    for (int i = 0; i < 1000 * 10000; ++i) {
        //LOGDEBUG("before Resume fun_co");
        co->Resume();
        //LOGDEBUG("after Resume fun_co");
        //sleep(1);
    }
}

void OnTestShareStackMem(int co_id) {
    while(true) {
        char buff[128];
        LOGDEBUG("co_id = " << co_id << ", buff ptr = " << (size_t)buff);
        xco::Coroutine::Yield();
    }
}

void test_share_stack_mem() {
    xco::Coroutine::StackMem share_mem;
    auto co1 = xco::Coroutine::Create(std::bind(OnTestShareStackMem, 1), &share_mem);
    auto co2 = xco::Coroutine::Create(std::bind(OnTestShareStackMem, 2), &share_mem);
    while(true) {
        co1->Resume();
        co2->Resume();
    }
}

int main() {
    test_resume();
    return 0;
}