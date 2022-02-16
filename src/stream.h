/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/13
 *================================================================*/
#pragma once

#include <memory>
#include "bytearray.h"

XCO_NAMESPAVE_START
class Stream {
public:
    typedef std::shared_ptr<Stream> Ptr;

public:
    virtual ~Stream() {};

public:
    /**
      * @brief 读数据
      * @param[out] buffer 接收数据的内存
      * @param[in] length 接收数据的内存大小
      * @return
      *      @retval >0 返回接收到的数据的实际大小
      *      @retval =0 被关闭
      *      @retval <0 出现流错误
      */
    virtual int Read(void* buffer, size_t length) = 0;

    /**
     * @brief 读数据
     * @param[out] ba 接收数据的ByteArray
     * @param[in] length 接收数据的内存大小
     * @return
     *      @retval >0 返回接收到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int Read(ByteArray::Ptr ba, size_t length) = 0;

    /**
     * @brief 读固定长度的数据
     * @param[out] buffer 接收数据的内存
     * @param[in] length 接收数据的内存大小
     * @return
     *      @retval >0 返回接收到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int ReadFixSize(void* buffer, size_t length);

    /**
     * @brief 读固定长度的数据
     * @param[out] ba 接收数据的ByteArray
     * @param[in] length 接收数据的内存大小
     * @return
     *      @retval >0 返回接收到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int ReadFixSize(ByteArray::Ptr ba, size_t length);

    /**
     * @brief 写数据
     * @param[in] buffer 写数据的内存
     * @param[in] length 写入数据的内存大小
     * @return
     *      @retval >0 返回写入到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int Write(const void* buffer, size_t length) = 0;

    /**
     * @brief 写数据
     * @param[in] ba 写数据的ByteArray
     * @param[in] length 写入数据的内存大小
     * @return
     *      @retval >0 返回写入到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int Write(ByteArray::Ptr ba, size_t length) = 0;

    /**
     * @brief 写固定长度的数据
     * @param[in] buffer 写数据的内存
     * @param[in] length 写入数据的内存大小
     * @return
     *      @retval >0 返回写入到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int WriteFixSize(const void* buffer, size_t length);

    /**
     * @brief 写固定长度的数据
     * @param[in] ba 写数据的ByteArray
     * @param[in] length 写入数据的内存大小
     * @return
     *      @retval >0 返回写入到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int WriteFixSize(ByteArray::Ptr ba, size_t length);

    virtual int ReadHandle(char* buf, size_t buf_size, std::function<int(int read_len)> handle);

    /**
     * @brief 关闭流
     */
    virtual void Close() = 0;

};//class Stream

XCO_NAMESPAVE_END