/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ex_util.cpp
      	创 建 者：xiesenxin
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <boost/lexical_cast.hpp>
#include <execinfo.h>
#include <sys/time.h>
#include <cxxabi.h>
#include <sys/wait.h>
#include <sys/syscall.h>

//=================================操作模板 <start>=================================//
/**
 * @brief 获取Map中的key值,并转成对应类型,返回是否成功
 * @param[in] m Map数据结构
 * @param[in] key 关键字
 * @param[out] val 保存转换后的值
 * @param[in] def 默认值
 * @retval true 转换成功, val 为对应的值
 * @retval false 不存在或者转换失败 val = def
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
