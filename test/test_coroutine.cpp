/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�echo_server.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/2/7
 *================================================================*/

#include "coroutine.h"
#include <iostream>
#include <unistd.h>

void fun(void* arg) {
    while(true) {
        //LOGDEBUG("before Yield fun_co");
        xco::Coroutine::Yield();
        //LOGDEBUG("after Yield fun_co");
    }
}

void test_resume() {
    xco::Coroutine fun_co(fun);
    for (int i = 0; i < 1000 * 10000; ++i) {
        //LOGDEBUG("before Resume fun_co");
        fun_co.Resume();
        //LOGDEBUG("after Resume fun_co");
        //sleep(1);
    }
}

int main() {
    test_resume();
    return 0;
}