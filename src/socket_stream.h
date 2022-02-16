/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�socket_stream.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/16
 *================================================================*/
#pragma once

#include "stream.h"
#include "socket.h"
#include "common.h"

XCO_NAMESPAVE_START
class SocketStream : public Stream, public BaseDump{
public:
    typedef std::shared_ptr<SocketStream> Ptr;

public:
    SocketStream(Socket::Ptr socket, bool auto_close = false);
    ~SocketStream();

public:
    int Read(void *buffer, size_t length) override;

    int Read(ByteArray::Ptr ba, size_t length) override;

    int Write(const void *buffer, size_t length) override;

    int Write(ByteArray::Ptr ba, size_t length) override;

    void Close() override;

    bool IsConnected() const;

private:
    Socket::Ptr socket_;
    bool auto_close_;

public:
    FUNCTION_BUILDER_VAR_GETTER(Socket, socket_);
};

XCO_NAMESPAVE_END