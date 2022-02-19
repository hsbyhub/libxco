/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：common.cpp
      	创 建 者：hsby
      	创建日期：2022/2/10
 *================================================================*/
#include "common.h"

static int g_xco_log_level = 1;

void DumpFmtDate(std::ostream& os){
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