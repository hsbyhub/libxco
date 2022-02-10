/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�common.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/9
 *================================================================*/
#pragma once

#include <iostream>
#include <sys/time.h>

// �����ռ�
#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }

// ������־
//#define LOGDEBUG(msg) std::cout << __FILE__ << ":" << __LINE__ << "|" << msg << std::endl << std::flush
#define LOGDEBUG(msg)

// GCCԤ����Ϣ(CPU��ˮ�߼������������)
#if defined(__GNUC__) || defined(__llvm__)
#   define XCO_LICKLY(x)     __builtin_expect(!!(x), 1)
#   define XCO_UNLICKLY(x)   __builtin_expect(!!(x), 0)
#else
#   define XCO_LICKLY(x)     (x)
#   define XCO_UNLICKLY(x)   (x)
#endif

/**
 * @brief ��ȡ��ǰʱ�䣬����
 */
inline uint64_t TimeStampMs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

/**
 * @brief ��ȡ��ǰʱ�䣬΢��
 */
inline uint64_t TimeStampUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

/**
 * @brief ��timevalת��Ϊ����ʱ���
 */
inline int64_t TimevalToMs(const timeval &tv) {
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief ����ģʽ
 */
template<typename T, typename X = void, int N = 0>
class Singleton{
public:
    static T& Instance(){
        static T v;
        return v;
    }
};