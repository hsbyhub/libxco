/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：struct.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <iostream>

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