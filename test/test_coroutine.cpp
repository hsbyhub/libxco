/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/7
 *================================================================*/

#include "coroutine.h"
#include <iostream>
#include <unistd.h>

void* fun1(void* arg) {
    while (true) {
        sleep(1);
        LOGDEBUG("I am fun1");
        auto next_co = (xco::Coroutine*)arg;
        next_co->Resume();
    }
    return nullptr;
}

void* fun2(void* arg) {
    while (true) {
        sleep(1);
        LOGDEBUG("I am fun1");
        auto next_co = (xco::Coroutine*)arg;
        next_co->Resume();
    }
    return nullptr;
}

int main() {
    xco::Coroutine fun1_co(fun1);
    xco::Coroutine fun2_co(fun2);
    fun1_co.cb_arg_ = &fun2_co;
    fun2_co.cb_arg_ = &fun1_co;
    LOGDEBUG("Resume fun1_co");
    fun1_co.Resume();
    return 0;
}