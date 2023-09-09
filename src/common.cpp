/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：common.cpp
      	创 建 者：xiesenxin
      	创建日期：2022/2/10
 *================================================================*/
#include "common.h"

XCO_NAMESPAVE_START

static int g_xco_log_level = 1;

void FormatDate(std::ostream& os){
    char buf[64];
    time_t t = time(0);
    struct tm *tm = localtime(&t);
    strftime(buf, sizeof(buf), "%X", tm);
    os << buf;
}

int GetLogLevel() {
    return g_xco_log_level;
}

void SetLogLevel(int level) {
    g_xco_log_level = level;
}

XCO_NAMESPAVE_END