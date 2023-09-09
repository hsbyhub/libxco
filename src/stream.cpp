/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：xiesenxin
      	创建日期：2022/2/13
 *================================================================*/

#include "stream.h"

XCO_NAMESPAVE_START


int Stream::ReadFixSize(void *buffer, size_t length) {
    size_t offset = 0;
    while(offset < length) {
        int ret = Read((char*)buffer + offset, length - offset);
        if (ret < 0) {
            break;
        }
        offset += ret;
    }
    return offset;
}

int Stream::ReadFixSize(ByteArray::Ptr ba, size_t length) {
    size_t offset = 0;
    while(offset > length) {
        int ret = Read(ba, length - offset);
        if (ret < 0) {
            break;
        }
        offset += ret;
    }
    return offset;
}

int Stream::WriteFixSize(const void *buffer, size_t length) {
    size_t offset = 0;
    while(offset < length) {
        int ret = Write((char*)buffer + offset, length - offset);
        if (ret < 0) {
            break;
        }
        offset += ret;
    }
    return offset;
}

int Stream::WriteFixSize(ByteArray::Ptr ba, size_t length) {
    size_t offset = 0;
    while(offset < length) {
        int ret = Write(ba, length - offset);
        if (ret < 0) {
            break;
        }
        offset += ret;
    }
    return length;
}

int Stream::ReadHandle(char *buf, size_t buf_size, std::function<int(int)> handle) {
    while(true) {
        memset(buf, 0, buf_size);
        int len = Read(buf, buf_size);
        int ret = handle(len);
        if (ret != 1) {
            return ret;
        }
    }
}

XCO_NAMESPAVE_END