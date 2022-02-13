/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�echo_server.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/2/13
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
      * @brief ������
      * @param[out] buffer �������ݵ��ڴ�
      * @param[in] length �������ݵ��ڴ��С
      * @return
      *      @retval >0 ���ؽ��յ������ݵ�ʵ�ʴ�С
      *      @retval =0 ���ر�
      *      @retval <0 ����������
      */
    virtual int Read(void* buffer, size_t length) = 0;

    /**
     * @brief ������
     * @param[out] ba �������ݵ�ByteArray
     * @param[in] length �������ݵ��ڴ��С
     * @return
     *      @retval >0 ���ؽ��յ������ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int Read(ByteArray::Ptr ba, size_t length) = 0;

    /**
     * @brief ���̶����ȵ�����
     * @param[out] buffer �������ݵ��ڴ�
     * @param[in] length �������ݵ��ڴ��С
     * @return
     *      @retval >0 ���ؽ��յ������ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int ReadFixSize(void* buffer, size_t length);

    /**
     * @brief ���̶����ȵ�����
     * @param[out] ba �������ݵ�ByteArray
     * @param[in] length �������ݵ��ڴ��С
     * @return
     *      @retval >0 ���ؽ��յ������ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int ReadFixSize(ByteArray::Ptr ba, size_t length);

    /**
     * @brief д����
     * @param[in] buffer д���ݵ��ڴ�
     * @param[in] length д�����ݵ��ڴ��С
     * @return
     *      @retval >0 ����д�뵽�����ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int Write(const void* buffer, size_t length) = 0;

    /**
     * @brief д����
     * @param[in] ba д���ݵ�ByteArray
     * @param[in] length д�����ݵ��ڴ��С
     * @return
     *      @retval >0 ����д�뵽�����ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int Write(ByteArray::Ptr ba, size_t length) = 0;

    /**
     * @brief д�̶����ȵ�����
     * @param[in] buffer д���ݵ��ڴ�
     * @param[in] length д�����ݵ��ڴ��С
     * @return
     *      @retval >0 ����д�뵽�����ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int WriteFixSize(const void* buffer, size_t length);

    /**
     * @brief д�̶����ȵ�����
     * @param[in] ba д���ݵ�ByteArray
     * @param[in] length д�����ݵ��ڴ��С
     * @return
     *      @retval >0 ����д�뵽�����ݵ�ʵ�ʴ�С
     *      @retval =0 ���ر�
     *      @retval <0 ����������
     */
    virtual int WriteFixSize(ByteArray::Ptr ba, size_t length);

    virtual int ReadHandle(char* buf, size_t buf_size, std::function<int(int read_len)> handle);

    /**
     * @brief �ر���
     */
    virtual void Close() = 0;

};//class Stream

XCO_NAMESPAVE_END