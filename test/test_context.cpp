/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_context.cpp
      	创 建 者：hsby
      	创建日期：2022/2/8
 *================================================================*/
#include <ucontext.h>
#include <memory>
#include <cstring>
#include "common.h"

const int STACK_SIZE = 128 * 1000;
using CbType = void(*)();

ucontext_t* main_ctx = nullptr;
ucontext_t* fun_ctx = nullptr;

ucontext_t* MakeCtx(CbType cb = nullptr) {
    ucontext_t* ctx = (ucontext_t*)calloc(1, sizeof(ucontext_t));
    char* stack = (char*)malloc(STACK_SIZE);
    if (getcontext(ctx)) {
        return nullptr;
    }
    if (cb) {
        ctx->uc_link = nullptr;
        ctx->uc_stack.ss_sp = stack;
        ctx->uc_stack.ss_size = STACK_SIZE;
        makecontext(ctx, cb, 0);
    }
    return ctx;
}

void DestoryCtx(ucontext_t* ctx) {
    if (ctx) {
        if (ctx->uc_stack.ss_sp) {
            free(ctx->uc_stack.ss_sp);
        }
        free(ctx);
    }
}

void fun() {
    while(true) {
        //LOGDEBUG("before swap to main_ctx");
        swapcontext(fun_ctx, main_ctx);
        //LOGDEBUG("after swap to main_ctx");
    }
}

void test_context() {
    main_ctx = MakeCtx();
    fun_ctx = MakeCtx(fun);
    for (int i = 0; i < 1000 * 10000; ++i) {
        //LOGDEBUG("before swap to fun_ctx");
        swapcontext(main_ctx, fun_ctx);
        //LOGDEBUG("after swap to fun_ctx");
    }
    DestoryCtx(main_ctx);
    DestoryCtx(fun_ctx);
}

int main() {
    test_context();
    return 0;
}