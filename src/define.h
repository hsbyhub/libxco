/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/1/27
 *================================================================*/
#pragma once

#include <iostream>

#define XCO_NAMESPAVE_START namespace xco{
#define XCO_NAMESPAVE_END }

#define LOGDEBUG(msg) std::cout << __FILE__ << ":" << __LINE__ << "|" << msg << std::endl << std::flush