/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/7
 *================================================================*/

#include "coroutine.h"
#include <iostream>
#include <unistd.h>

void fun(void* arg) {
    while(true) {
        //LOGDEBUG("before Yield fun_co");
        LOGDEBUG((size_t)arg);
        xco::Coroutine::Yield();
        //LOGDEBUG("after Yield fun_co");
    }
}

void test_resume() {
    auto co = xco::Coroutine::Create(fun, (void*)100);
    for (int i = 0; i < 1000 * 10000; ++i) {
        //LOGDEBUG("before Resume fun_co");
        co->Resume();
        //LOGDEBUG("after Resume fun_co");
        //sleep(1);
    }
}

void fun1(void* arg) {
    //size_t co_id = (size_t)arg;
    //while(true) {
    //    char buff[128];
    //    LOGDEBUG("co_id = " << co_id << ", buff ptr = " << (size_t)buff);
    //    xco::Coroutine::Yield();
    //}
}

void test_share_stack_mem() {
    xco::Coroutine::StackMem share_mem;
    auto co1 = xco::Coroutine::Create(fun1, (void*)1, &share_mem);
    auto co2 = xco::Coroutine::Create(fun1, (void*)2, &share_mem);
    while(true) {
        co1->Resume();
        co2->Resume();
    }
}

int main() {
    test_resume();
    return 0;
}