/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：socket_stream.cpp
      	创 建 者：hsby
      	创建日期：2022/2/16
 *================================================================*/
#include "socket_stream.h"

XCO_NAMESPAVE_START
SocketStream::SocketStream(Socket::Ptr socket, bool auto_close)
    : socket_(socket), auto_close_(auto_close){
}

SocketStream::~SocketStream() {
    if (auto_close_ && socket_) {
        socket_->Close();
    }
}

int SocketStream::Read(void *buffer, size_t length) {
    if (!IsConnected() || !buffer) {
        return -1;
    }
    return socket_->Recv((char*)buffer, length);
}

int SocketStream::Read(ByteArray::Ptr ba, size_t length) {
    if (!IsConnected() || !ba) {
        return -1;
    }
    std::vector<iovec> iovs;
    ba->GetWriteIovecs(iovs, length);
    int ret = socket_->Recv(&iovs[0], iovs.size());
    ba->AddWritePos(ret);
    return ret;
}

int SocketStream::Write(const void *buffer, size_t length) {
    if (!IsConnected() || !buffer) {
        return -1;
    }
    return socket_->Send((const char*)buffer, length);
}

int SocketStream::Write(ByteArray::Ptr ba, size_t length) {
    if (!ba) {
        return -1;
    }
    std::vector<iovec> iovs;
    ba->GetReadIovecs(iovs, length);
    int ret = socket_->Send(&iovs[0], iovs.size());
    ba->AddReadPos(ret);
    return ret;
}

void SocketStream::Close() {
    if (socket_) {
        socket_->Close();
    }
}

bool SocketStream::IsConnected() const {
    return socket_ && socket_->IsConnected();
}
XCO_NAMESPAVE_END