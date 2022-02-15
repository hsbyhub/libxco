/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�common.cpp
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
 *================================================================*/
#include "common.h"
#include <sys/time.h>
#include <execinfo.h>
#include <sys/syscall.h>

void DumpFmtDate(std::ostream& os){
    char buf[64];
    time_t t = time(0);
    struct tm *tm = localtime(&t);
    strftime(buf, sizeof(buf), "%X", tm);
    os << buf;
}

static int g_xco_log_level = 1;
int GetLogLevel() {
    return g_xco_log_level;
}

void SetLogLevel(int level) {
    g_xco_log_level = level;
}

/**
 * @brief ȡ�õ�ǰ�߳�Id
 */
int GetThisThreadId(){
    return syscall(SYS_gettid);
}

/**
 * @brief ��ȡ��ǰ��ջ��Ϣ
 */
void BackTrace(std::vector<std::string>& res, int size, int skip) {
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
 * @brief ��ȡ��ǰ��ջ��Ϣ
 * @return std::string
 */
std::string BackTraceString(int size, int skip, std::string prefix) {
    void** buffer = (void**) malloc(sizeof(void*) * size);
    int s = ::backtrace(buffer , size);
    char** strings = backtrace_symbols(buffer, s);
    std::stringstream  ss;
    for (int i = skip; i < s; ++i) {
        ss << prefix << strings[i];
    }
    return ss.str();
}

/**
 * @brief ��ȡ��ǰʱ�䣬����
 */
uint64_t TimeStampMs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
};

/**
 * @brief ��ȡ��ǰʱ�䣬΢��
 */
uint64_t TimeStampUs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
};

/**
 * @brief ��ȡ�߳����е���ǰʱ�̵�ʱ��, ΢��
 */
uint64_t GetRealTimeUs() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec*1000*1000 + tp.tv_nsec / 1000;
}

int64_t TimevalToMs(const timeval &tv) {
    return tv.tv_sec * 1000 + tv.tv_usec;
}