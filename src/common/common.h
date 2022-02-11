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
#include "cxxabi.h"

// 命名空间
#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }

// 日志
int GetLogLevel();
void SetLogLevel(int level);
#define DEBUG 1
#define WARN  2
#define ERROR 3
#define FATAL 4
#define LOG_IF_LEVEL(level, msg)                                \
        if (level >= GetLogLevel()){                            \
             std::cout  << __FILE__ << ":" << __LINE__ << "|"   \
                        << #level << "|"                        \
                        << msg << std::endl << std::flush;      \
        }
#define LOGDEBUG(msg)   LOG_IF_LEVEL(DEBUG, msg)
#define LOGWARN(msg)    LOG_IF_LEVEL(WARN, msg)
#define LOGERROR(msg)   LOG_IF_LEVEL(ERROR, msg)
#define LOGFATAL(msg)   LOG_IF_LEVEL(FATAL, msg)

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

/**
 * @brief 不可拷贝基类
 */
class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

/**
 * @brief ToString
 */
class BaseDump {
public:
    BaseDump(){}
    virtual ~BaseDump(){}
    virtual void Dump(std::ostream& os) const{}
    std::string ToString() const{
        std::stringstream ss;
        Dump(ss);
        return ss.str();
    }
};
inline std::ostream &operator<<(std::ostream &os, const BaseDump& base_dump) {
    base_dump.Dump(os);
    return os;
}

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
    if (XCO_UNLICKLY(!(arg))){                   \
        XCO_LOG_ERROR(XCO_LOG_NAME("system")) << "ASSERTION:  "#arg" "\
                << "\nbacktrace: "                              \
                << XCO::BackTraceString(100, 1, "\n        "); \
        assert(0);                                            \
    }

/**
 * @brief 断言，自动输出最多100层堆栈信息
 */
#define XCO_ASSERT_MSG(arg, msg)                   \
    if (XCO_UNLICKLY(!(arg))){                                    \
        XCO_LOG_ERROR(XCO_LOG_NAME("system")) << "ASSERTION:  "#arg" " \
                << "\nmessage: " << msg                          \
                << "\nbacktrace: "                               \
                << XCO::BackTraceString(100, 1, "\n        ");  \
        assert(0);                                             \
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

//================================= 自动生成代码 start =================================//
/**
 * @brief 生成变量的setter
 */
#define FUNCTION_BUILDER_VAR_SETTER(name, var)  \
void Set##name(const decltype(var)& value) {    \
    (var) = value;                              \
}

/**
 * @brief 生成变量的getter
 */
#define FUNCTION_BUILDER_VAR_GETTER(name, var)  \
decltype(var) Get##name() const {               \
    return var;                                 \
}

/**
 * @brief 生成变量的setter和getter
 */
#define FUNCTION_BUILDER_VAR(name, var)         \
        FUNCTION_BUILDER_VAR_SETTER(name, var)  \
        FUNCTION_BUILDER_VAR_GETTER(name, var)

/**
 * @brief 生成map的setter
 */
#define FUNCTION_BUILDER_MAP_SETTER(name, mp)                                   \
void Set##name(const typename decltype(mp)::key_type& key,                      \
               const typename decltype(mp)::value_type::second_type& value) {   \
    (mp)[key] = value;                                                          \
}

/**
 * @brief 生成map的getter
 */
#define FUNCTION_BUILDER_MAP_GETTER(name, mp)                                   \
typename decltype(mp)::value_type::second_type                                  \
Get##name(const typename decltype(mp)::key_type& key) const {                   \
    auto it = (mp).find(key);                                                   \
    return it == (mp).end() ? decltype(mp)::key_type() : it->second;            \
}

/**
 * @brief 生成map的getter with default
 */
#define FUNCTION_BUILDER_MAP_GETTER_DEFAULT(name, mp)                           \
typename decltype(mp)::value_type::second_type                                  \
Get##name(const typename decltype(mp)::key_type& key,                           \
          const typename decltype(mp)::value_type::second_type& def) const {    \
    auto it = (mp).find(key);                                                   \
    return it == (mp).end() ? def : it->second;                                 \
}

/**
 * @brief 生成map的del
 */
#define FUNCTION_BUILDER_MAP_DEL(name, mp)                                      \
void Del##name(const decltype(mp)::key_type& key) {                             \
    (mp).erase(key);                                                            \
}

/**
 * @brief 生成map的has
 */
#define FUNCTION_BUILDER_MAP_HAS(name, mp)                                      \
bool Has##name(const decltype(mp)::key_type& key) const {                       \
    return (mp).find(key) != (mp).end();                                        \
}

/**
 * @brief 生成map的所有操作函数
 */
#define FUNCTION_BUILDER_MAP(name, mp)                  \
        FUNCTION_BUILDER_MAP_SETTER(name, mp)           \
        FUNCTION_BUILDER_MAP_GETTER(name, mp)           \
        FUNCTION_BUILDER_MAP_GETTER_DEFAULT(name, mp)   \
        FUNCTION_BUILDER_MAP_DEL(name, mp)              \
        FUNCTION_BUILDER_MAP_HAS(name, mp)              \

/**
 * @brief 生成可以强制转换为某种类型的Getter
 * @param[in] key:键值
 * @param[in] def:默认值
 * @param[out] val:返回值
 * @return T类型的返回值
 */
#define FUNCTION_BUILDER_MAP_AS(name, mp)                                       \
template<class T>                                                               \
T Get##name##As(const typename decltype(mp)::key_type& key,                     \
                const T& def = T()) const {                                     \
    return GetAsFromMap(mp, key, def);                                          \
}                                                                               \
                                                                                \
template<class T>                                                               \
bool Get##name##AsWithCheck(const typename decltype(mp)::key_type& key, T& val, \
                            const T& def = T()) const {                         \
    return GetAsFromMapWithCheck(mp, key, val, def);                            \
}

/**
 * @brief 生成可以强制转换为某种类型的Getter
 * @param[in] key:键值
 * @param[in] def:默认值
 * @param[out] val:返回值
 * @return T类型的返回值
 */
#define FUNCTION_BUILDER_MAP_AS_WITH_PREDEAL(name, mp, predel)                  \
template<class T>                                                               \
T Get##name##As(const typename decltype(mp)::key_type& key,                     \
                const T& def = T()) {                                           \
    predel;                                                                     \
    return GetAsFromMap(mp, key, def);                                          \
}                                                                               \
                                                                                \
template<class T>                                                               \
bool Get##name##AsWithCheck(const typename decltype(mp)::key_type& key, T& val, \
                            const T& def = T()) {                               \
    predel;                                                                     \
    return GetAsFromMapWithCheck(mp, key, val, def);                            \
}
//=================================自动生成代码 end=================================//

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

//=================================线程相关 <start>=================================//
/**
 * @brief 获取当前线程Id
 */
int GetThisThreadId();

/**
 * @brief 提取当前堆栈信息
 */
void BackTrace(std::vector<std::string>& , int size = 64, int skip = 1);

/**
 * @brief 提取当前堆栈信息
 */
std::string BackTraceString( int size = 100, int skip = 1, std::string post = "");
//=================================线程相关 <end>=================================//

//=================================时间相关 <start>=================================//
/**
 * @brief 获取当前时间，毫秒
 */
uint64_t TimeStampMs();

/**
 * @brief 获取当前时间，微秒
 */
uint64_t TimeStampUs();

/**
 * @brief 获取线程运行到当前时刻的时间
 */
uint64_t GetRealTimeUs();

/**
 * @brief 将timeval转化为毫秒
 */
int64_t TimevalToMs(const timeval& tv);

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
//=================================内存相关 <end>=================================//

//=================================二进制相关 <start>=================================//
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
//=================================二进制相关 <end>=================================//

template<typename Type>
const char* GetTypeName() {
    static const char* s_name = abi::__cxa_demangle(typeid(Type).name(), nullptr, nullptr, nullptr);
    return s_name;
}