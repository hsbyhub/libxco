/*================================================================*
        Copyright (C) 2021 All rights reserved, www.XCO.link.
      	文件名称：common.cpp
      	创 建 者：XCO
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

#include "util/ex_string.hpp"
#include "util/ex_fucntion.hpp"
#include "util/ex_struct.hpp"
#include "util/ex_util.hpp"
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <errno.h>
#include <assert.h>
#include <byteswap.h>
#include <stdint.h>
#include <boost/lexical_cast.hpp>
#include <csignal>
#include <sys/time.h>
#include <execinfo.h>
#include <cxxabi.h>

// 命名空间
#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }

// 字节序
#define XCO_ENDIAN_LITTLE 1
#define XCO_ENDIAN_BIG 2

// 日志
void FormatDate(std::ostream& os);
int GetLogLevel();
void SetLogLevel(int level);
#define XCO_DEBUG 1
#define XCO_WARN  2
#define XCO_ERROR 3
#define XCO_FATAL 4
#define XCO_LOG_IF_LEVEL(level, msg)                                \
        if (level >= GetLogLevel()){                            \
             std::cout  << __FILE__ << ":" << __LINE__ << "|";  \
             FormatDate(std::cout); std::cout << "|";          \
             std::cout  << #level << "|"                        \
                        << msg << std::endl << std::flush;      \
        }
#define XCO_LOGDEBUG(msg)   XCO_LOG_IF_LEVEL(XCO_DEBUG, msg)
#define XCO_LOGWARN(msg)    XCO_LOG_IF_LEVEL(XCO_WARN, msg)
#define XCO_LOGERROR(msg)   XCO_LOG_IF_LEVEL(XCO_ERROR, msg)
#define XCO_LOGFATAL(msg)   XCO_LOG_IF_LEVEL(XCO_FATAL, msg)

/**
 * @brief 提供GCC预测信息(CPU流水线技术下提高性能)
 */
#if defined(__GNUC__) || defined(__llvm__)
#   define XCO_LICKLY(x)     __builtin_expect(!!(x), 1)
#   define XCO_UNLICKLY(x)   __builtin_expect(!!(x), 0)
#else
#   define XCO_LICKLY(x)     (x)
#   define XCO_UNLICKLY(x)   (x)
#endif


//================================= 断言 start =================================//
/**
 * @brief 断言，自动输出最多100层堆栈信息
 */
#define XCO_ASSERT(arg)           \
    if (XCO_UNLICKLY(!(arg))){ \
        XCO_LOGFATAL("ASSERTION:  "#arg" " << "backtrace: " << BackTraceString(100, 1, "\n        ")); \
        assert(0);                                            \
    }

/**
 * @brief 断言，自动输出最多100层堆栈信息
 */
#define XCO_ASSERT_MSG(arg, msg)                   \
    if (XCO_UNLICKLY(!(arg))){                                    \
        XCO_LOG_FATAL("ASSERTION:  "#arg" "\
                << "\nbacktrace: "                              \
                << BackTraceString(100, 1, "\n        ")); \
        assert(0);                                            \
    }
//================================= 断言 end =================================//

//=================================字节序 start=================================//
/**
 * @brief 字节序转换
 * @param[in]
 * @return
 */
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type ByteSwap(T value) {
    return static_cast<T>(bswap_16((uint16_t)value));
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type ByteSwap(T value) {
    return static_cast<T>(bswap_32((uint32_t)value));
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type ByteSwap(T value) {
    return static_cast<T>(bswap_64((uint64_t)value));
}

#if BYTE_ORDER == LITTLE_ENDIAN
#define XCO_ENDIAN XCO_ENDIAN_LITTLE
#else
#define XCO_ENDIAN XCO_ENDIAN_BIG
#endif

#if XCO_ENDIAN == XCO_ENDIAN_LITTLE
template<typename T> T ByteSwapOnLittleEndian(T t) {
    return ByteSwap(t);
}
template<typename T> T ByteSwapOnBigEndian(T t) {
    return t;
}
#else
template<typename T> T ByteSwapOnLittleEndian(T t) {
    return t;
}
template<typename T> T ByteSwapOnBigEndian(T t) {
    return ByteSwap(t);
}
#endif
//=================================字节序 end=================================//