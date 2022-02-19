/*================================================================*
        Copyright (C) 2021 All rights reserved, www.XCO.link.
      	文件名称：common.cpp
      	创 建 者：XCO
      	创建日期：2022/2/9
 *================================================================*/
#pragma once

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
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <execinfo.h>
#include "cxxabi.h"

// 命名空间
#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }


// 日志
void DumpFmtDate(std::ostream& os);
int GetLogLevel();
void SetLogLevel(int level);
#define XCO_DEBUG 1
#define XCO_WARN  2
#define XCO_ERROR 3
#define XCO_FATAL 4
#define LOG_IF_LEVEL(level, msg)                                \
        if (level >= GetLogLevel()){                            \
             std::cout  << __FILE__ << ":" << __LINE__ << "|";  \
             DumpFmtDate(std::cout); std::cout << "|";          \
             std::cout  << #level << "|"                        \
                        << msg << std::endl << std::flush;      \
        }
#define XCO_LOGDEBUG(msg)   LOG_IF_LEVEL(XCO_DEBUG, msg)
#define XCO_LOGWARN(msg)    LOG_IF_LEVEL(XCO_WARN, msg)
#define XCO_LOGERROR(msg)   LOG_IF_LEVEL(XCO_ERROR, msg)
#define XCO_LOGFATAL(msg)   LOG_IF_LEVEL(XCO_FATAL, msg)

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

/**
 * @brief 字节序类型
 */
#define XCO_ENDIAN_LITTLE 1
#define XCO_ENDIAN_BIG 2

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

//================================= 便捷表达 start =================================//
#define XCO_ERROR_EXP (std::string("errno=") + std::to_string(errno) + ", strerr=" + strerror(errno))
#define XCO_VARS_EXP(...) (ParseArgList(#__VA_ARGS__, 0, "", __VA_ARGS__))
#define XCO_FUNC_WITH_ARG_EXP(...) (ParseFuncExp(__PRETTY_FUNCTION__, XCO_VARS_EXP(__VA_ARGS__)))
#define XCO_FUNC_ERROR_EXP (std::string(__PRETTY_FUNCTION__ )+ " error, " + XCO_ERROR_EXP)
#define XCO_FUNC_ERROR_WITH_ARG_EXP(...) (XCO_FUNC_WITH_ARG_EXP(__VA_ARGS__) + " error, " + XCO_ERROR_EXP)

inline std::string ParseFuncExp(const std::string& func_name, const std::string& func_args_express) {
    std::stringstream ss;
    size_t i = 0;
    while(i < func_name.size()) {
        ss << func_name[i];
        if (func_name[i] == '(') break;
        i++;
    }
    ss << func_args_express;
    while(i < func_name.size()) {
        if (func_name[i] == ')') break;
        i++;
    }
    if (i < func_name.size()) {
        ss << func_name[i];
    }
    return ss.str();
}

inline bool IsArgListSplit(char ch) {
    return ch == ',';
}

// 终止遍历可变参数
inline void ParseArgList(std::stringstream& ss,
                         const std::string& format, int format_idx,
                         const std::string& prefix) {
}
// 遍历可变参数
template<typename VarType, typename ...VarTypes>
void ParseArgList(std::stringstream& ss,
                  const std::string& format, int format_idx,
                  const std::string& prefix,
                  VarType var, VarTypes...vars) {
    ss << prefix;
    bool found_name = false;
    for (; format_idx < format.size(); format_idx++) {
        char ch = format[format_idx];
        if (!found_name && !IsArgListSplit(ch)) {
            found_name = true;
        }
        if (found_name) {
            if (IsArgListSplit(ch))  {
                break;
            }
            ss << ch;
        }
    }
    ss << "=" << var;
    ParseArgList(ss, format, format_idx, ",", vars...);
}

template<typename VarType, typename ...VarTypes>
std::string ParseArgList(const std::string& format, int format_begin,
                         const std::string& prefix,
                         VarType var, VarTypes...vars) {
    std::stringstream ss;
    ParseArgList(ss, format, format_begin, prefix, var, vars...);
    return ss.str();
}
//================================= 便捷表达 end =================================//

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

XCO_NAMESPAVE_START

//=================================操作模板 <start>=================================//
/**
 * @brief 获取Map中的key值,并转成对应类型,返回是否成功
 * @param[in] m Map数据结构
 * @param[in] key 关键字
 * @param[out] val 保存转换后的值
 * @param[in] def 默认值
 * @return
 *      @retval true 转换成功, val 为对应的值
 *      @retval false 不存在或者转换失败 val = def
 */
template<class MapType, class T>
bool GetAsFromMapWithCheck(const MapType& m, const std::string& key, T& val, const T& def = T()) {
    auto it = m.find(key);
    if(it == m.end()) {
        val = def;
        return false;
    }
    try {
        val = boost::lexical_cast<T>(it->second);
        return true;
    } catch (...) {
        val = def;
    }
    return false;
}

/**
 * @brief 获取Map中的key值,并转成对应类型
 * @param[in] mp Map数据结构
 * @param[in] key 关键字
 * @param[in] def 默认值
 * @return 如果存在且转换成功返回对应的值,否则返回默认值
 */
template<class MapType, class T>
T GetAsFromMap(const MapType& mp, const std::string& key, const T& def = T()) {
    auto it = mp.find(key);
    if(it == mp.end()) {
        return def;
    }
    try {
        return boost::lexical_cast<T>(it->second);
    } catch (...) {
    }
    return def;
}

/**
 * @brief 从map中获取值
 * @param[in]
 * @return
 */
template<typename MapType>
typename MapType::value_type::second_type GetFromMap(MapType& mp, const typename MapType::key_type& key) {
    auto it = mp.find(key);
    return it == mp.end() ? typename MapType::value_type::second_type() : it->second;
}

/**
 * @brief 复制map
 */
template<typename MapType>
void CopyMap(const MapType& mp_src, MapType& mp_dst) {
    for (auto it : mp_src) {
        mp_dst[it.first] = it.second;
    }
}
//=================================操作模板 <end>=================================//

//=================================线程相关 <start>=================================//
/**
 * @brief 获取当前线程Id
 */
inline int GetThisThreadId(){
    return syscall(SYS_gettid);
}

/**
 * @brief 提取当前堆栈信息
 */
inline void BackTrace(std::vector<std::string>& res, int size, int skip) {
    void** buffer = (void**) malloc(sizeof(void*) * size);
    int s = ::backtrace(buffer, size);
    char** strings = backtrace_symbols(buffer, s);
    free(buffer);
    if (strings == nullptr) {
        throw std::runtime_error("In BackTrace() : backtrace error");
    }
    for (int i = skip; i < s; ++i) {
        res.push_back(strings[i]);
    }
    free(strings);
}

/**
 * @brief 提取当前堆栈信息
 */
inline std::string BackTraceString(int size, int skip, std::string prefix) {
    void** buffer = (void**) malloc(sizeof(void*) * size);
    int s = ::backtrace(buffer , size);
    char** strings = backtrace_symbols(buffer, s);
    std::stringstream  ss;
    for (int i = skip; i < s; ++i) {
        ss << prefix << strings[i];
    }
    return ss.str();
}
//=================================线程相关 <end>=================================//

//=================================时间相关 <start>=================================//
/**
 * @brief 获取当前时间，毫秒
 */
inline uint64_t TimeStampMs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
};

/**
 * @brief 获取当前时间，微秒
 */
inline uint64_t TimeStampUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
};

/**
 * @brief 获取线程运行到当前时刻的时间
 */
inline uint64_t GetRealTimeUs() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec*1000*1000 + tp.tv_nsec / 1000;
}

/**
 * @brief 将timeval转化为毫秒
 */
inline int64_t TimevalToMs(const timeval &tv) {
    return tv.tv_sec * 1000 + tv.tv_usec;
}

/**
 * @brief 获取时间间隔
 */
struct TimeInterval {
    TimeInterval() {
        Get();
    }
    time_t Get() {
        time_t t1 = TimeStampUs();
        time_t res = t1 - t0;
        t0 = t1;
        return res;
    }
    time_t t0;
};
inline std::ostream& operator<<(std::ostream& os, TimeInterval& ti) {
    return os << " Interval=" << ti.Get() << "us";
}
//=================================时间相关 <end>=================================//

//=================================内存相关 <start>=================================//
/**
 * @brief 将对象置零
 */
template<typename T>
void MemSetZero(T& t) {
    memset(&t, 0, sizeof(t));
}

/**
 * @brief 计算类型为T, 前缀为prefix_len的后缀掩码(例如 T=uint32_t, prefix_len=24, return 0.0.0.255)
 * @return
 */
template<typename T>
T CreateSufMask(int prefix_len) {
    return (1 << (sizeof(T) * 8 - prefix_len)) - 1;
}

/**
 * @brief 计算类型为T, 前缀为prefix_len的前缀掩码(例如 T=uint32_t, prefix_len=24, return 255.255.255.0)
 * @return
 */
template<typename T>
T CreatePreMask(int prefix_len) {
    return ~CreateSufMask<T>(prefix_len);
}

/**
 * @brief 计算位1的数量
 * @return
 */
template<typename T>
uint32_t GetOneBitCount(T mask) {
    uint32_t res = 0;
    while(mask) {
        res += mask & 1;
        mask >>= 1;
    }
    return res;
}

/**
 * @brief 输出整型值的二进制字符串
 */
template<typename Type>
std::string IntToBitString(Type v) {
    std::stringstream ss;
    for (int idx = (int)sizeof(v) * 8 - 1; idx >= 0; idx--) {
        ss << (v & ((Type)1 << idx) ? '1' : '0');
    }
    return ss.str();
}
//=================================内存相关 <end>=================================//

/**
 * @brief 获取类型名字符串
 */
template<typename Type>
const char* GetTypeName() {
    static const char* s_name = abi::__cxa_demangle(typeid(Type).name(), nullptr, nullptr, nullptr);
    return s_name;
}

/**
 * @brief 创建多进程程序
 */
inline void MultiProcess(int process_cnt, std::function<void()> fun, void(*on_main_int)(int) = nullptr, void(*on_child_int)(int) = nullptr) {
    if (on_main_int) {
        signal(SIGINT, on_main_int);
    }
    for (int i = 0; i < process_cnt; ++i) {
        int ret = fork();
        if (ret != 0) {
            continue;
        } else {
            fun();
        }
    }
    wait(nullptr);
}

XCO_NAMESPAVE_END