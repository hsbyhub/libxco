/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：scheduler.h
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

#include "coroutine.h"
#include <queue>

XCO_NAMESPAVE_START

class Scheduler {
public:
    /**
     * @brief 构造函数
     */
    Scheduler();

    /**
     * @brief 析构函数
     */
    virtual ~Scheduler();

public:
    /**
     * @brief 开始
     */
    virtual void Start();

protected:
    /**
     * @brief 主循环
     */
    void OnLoop();

    /**
     * @brief 闲置处理
     */
    virtual void OnIdle();

public:
    /**
     * @brief 获取当前调度器
     */
    static Scheduler* GetCurScheduler();

    /**
     * @brief 调度
     * @param[in] coc 协程或回调
     */
    static void Schedule(Coroutine::Ptr co);

private:
    Coroutine::Ptr                 loop_co_    = nullptr;  // 主循环协程
    Coroutine::Ptr                 idle_co_    = nullptr;  // 闲置协程
    std::queue<Coroutine::Ptr>       co_list_;
};

XCO_NAMESPAVE_END