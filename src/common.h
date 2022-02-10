/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：common.cpp
      	创 建 者：hsby
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

#include <iostream>
#include <sys/time.h>

// 命名空间
#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }

// 调试日志
//#define LOGDEBUG(msg) std::cout << __FILE__ << ":" << __LINE__ << "|" << msg << std::endl << std::flush
#define LOGDEBUG(msg)

// GCC预测信息(CPU流水线技术下提高性能)
#if defined(__GNUC__) || defined(__llvm__)
#   define XCO_LICKLY(x)     __builtin_expect(!!(x), 1)
#   define XCO_UNLICKLY(x)   __builtin_expect(!!(x), 0)
#else
#   define XCO_LICKLY(x)     (x)
#   define XCO_UNLICKLY(x)   (x)
#endif

/**
 * @brief 获取当前时间，毫秒
 */
inline uint64_t TimeStampMs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

/**
 * @brief 获取当前时间，微秒
 */
inline uint64_t TimeStampUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

/**
 * @brief 将timeval转化为毫秒时间戳
 */
inline int64_t TimevalToMs(const timeval &tv) {
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief 单例模式
 */
template<typename T, typename X = void, int N = 0>
class Singleton{
public:
    static T& Instance(){
        static T v;
        return v;
    }
};