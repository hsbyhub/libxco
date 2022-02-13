/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�echo_server.cc
      	�� �� �ߣ���ɭ����
      	�������ڣ�2022/2/13
 *================================================================*/
#pragma once

#include <stdint.h>
#include <cstddef>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"

XCO_NAMESPAVE_START

class ByteArray : public BaseDump{
public:
    typedef std::shared_ptr<ByteArray> Ptr;

    struct Node {
        Node(size_t s = 0);
        ~Node();

        char* const data;
        const size_t cap;
        Node* next = nullptr;
    };
    typedef std::function<void(Node* node, size_t node_pos, size_t node_len, size_t opt_size)> IterOptFuncType;

public:
    ByteArray(size_t nodesize = 4096);
    ~ByteArray();

    static ByteArray::Ptr Create(size_t s = 4096) {
        return std::make_shared<ByteArray>(s);
    }

public:
    void Dump(std::ostream &os) const override;

private:

    /**
     * @brief �����ڴ��Ĵ�С
     */
    size_t GetNodeSize() const { return node_size_;}

    /**
     * @brief ��ȡ�Ŀ�д����
     */
    size_t GetCapSize() const;

    /**
     * @brief ��ȡ�ɶ�����������
     */
    size_t GetDataSize() const;

    /**
     * @brief ��������, ���ʣ������������Ҫ���������޶���
     * @param[in] need_cap:��Ҫ������
     */
    void FitCapacity(size_t need_cap);

    Node* GetNode(size_t pos) const;

    size_t GetNodePos(size_t pos) const;

    /**
     * @brief ����
     * @param[in]
     * @return
     */
    static size_t Iter(Node** node, size_t* pos, size_t len, size_t node_size, IterOptFuncType opt_func);
public:

    /**
     * @brief ���ByteArray
     * @post m_position = 0, m_size = 0
     */
    void Clear();

    /**
     * @brief �Ƿ���С��
     */
    bool IsLittleEndian() const;

    /**
     * @brief �����Ƿ�ΪС��
     */
    void SetIsLittleEndian(bool val);

    /**
     * @brief ��ȡ��ǰдλ��
     */
    size_t GetWritePos() const { return write_pos_;}

    /**
     * @brief ���õ�ǰдλ��
     */
    void SetWritePos(size_t pos);

    void AddWritePos(size_t count);

    /**
     * @brief ��ȡ��ǰ��λ��
     */
    size_t GetReadPos() const { return read_pos_;}

    /**
     * @brief ���õ�ǰ��λ��
     */
    void SetReadPos(size_t pos);

    /**
     * @brief ���ӵ�ǰ��λ��
     */
    void AddReadPos(size_t count);

public:
    /**
     * @brief �����������
     */
    std::string DataToTextString() const;

    /**
     * @brief ��ByteArray���������[m_position, m_size)ת��16���Ƶ�std::string(��ʽ:FF FF FF)
     */
    std::string DataToHexString() const;

    /**
     * @brief ��ByteArray������д�뵽�ļ���
     * @param[in] name �ļ���
     */
    bool WriteToFile(const std::string& name) const;

    /**
     * @brief ���ļ��ж�ȡ����
     * @param[in] name �ļ���
     */
    bool ReadFromFile(const std::string& name);

    /**
     * @brief ��ȡ�ɶ�ȡ�Ļ���,�����iovec����
     * @param[out] iovs ����ɶ�ȡ���ݵ�iovec����
     * @param[in] len ��ȡ���ݵĳ���,���len > getReadSize() �� len = getReadSize()
     * @return ����ʵ�����ݵĳ���
     */
    uint64_t GetReadIovecs(std::vector<iovec>& iovs, uint64_t len = ~0ull) const;

    /**
     * @brief ��ȡ�ɶ�ȡ�Ļ���,�����iovec����,��positionλ�ÿ�ʼ
     * @param[out] buffers ����ɶ�ȡ���ݵ�iovec����
     * @param[in] len ��ȡ���ݵĳ���,���len > getReadSize() �� len = getReadSize()
     * @param[in] read_pos ��ȡ���ݵ�λ��
     * @return ����ʵ�����ݵĳ���
     */
    uint64_t GetReadIovecs(std::vector<iovec>& buffers, uint64_t len, uint64_t read_pos) const;

    /**
     * @brief ��ȡ��д��Ļ���,�����iovec����
     * @param[out] buffers �����д����ڴ��iovec����
     * @param[in] len д��ĳ���
     * @return ����ʵ�ʵĳ���
     * @post ���(m_position + len) > m_capacity �� m_capacity����N���ڵ�������len����
     */
    uint64_t GetWriteIovecs(std::vector<iovec>& buffers, uint64_t len);

    /**
     * @brief д
     * @param[in] buf:д�������
     * @param[in] len:д������ݴ�С
     * @return ʵ��д��ĳ���
     */
    void Write(const void* buf, size_t len);

    /**
     * @brief ��
     * @param[in] buf:����������
     * @param[in] len:���������ݴ�С
     * @return ʵ�ʶ����Ĵ�С
     */
    size_t Read(void* buf, size_t len);

    /**
     * @brief ��pos��ʼ��(���ı��λ��)
     * @param[in] buf:����������
     * @param[in] len:���������ݴ�С
     * @return ʵ�ʶ����Ĵ�С
     */
    size_t Read(void* buf, size_t len, size_t pos) const;

    /**
     * @brief д��̶�����int8_t���͵�����
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFint8  (int8_t value);
    /**
     * @brief д��̶�����uint8_t���͵�����
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFuint8 (uint8_t value);
    /**
     * @brief д��̶�����int16_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFint16 (int16_t value);
    /**
     * @brief д��̶�����uint16_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFuint16(uint16_t value);

    /**
     * @brief д��̶�����int32_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFint32 (int32_t value);

    /**
     * @brief д��̶�����uint32_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFuint32(uint32_t value);

    /**
     * @brief д��̶�����int64_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFint64 (int64_t value);

    /**
     * @brief д��̶�����uint64_t���͵�����(���/С��)
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFuint64(uint64_t value);

    /**
     * @brief д���з���Varint32���͵�����
     * @post m_position += ʵ��ռ���ڴ�(1 ~ 5)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteInt32  (int32_t value);
    /**
     * @brief д���޷���Varint32���͵�����
     * @post m_position += ʵ��ռ���ڴ�(1 ~ 5)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteUint32 (uint32_t value);

    /**
     * @brief д���з���Varint64���͵�����
     * @post m_position += ʵ��ռ���ڴ�(1 ~ 10)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteInt64  (int64_t value);

    /**
     * @brief д���޷���Varint64���͵�����
     * @post m_position += ʵ��ռ���ڴ�(1 ~ 10)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteUint64 (uint64_t value);

    /**
     * @brief д��float���͵�����
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteFloat  (float value);

    /**
     * @brief д��double���͵�����
     * @post m_position += sizeof(value)
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteDouble (double value);

    /**
     * @brief д��std::string���͵�����,��uint16_t��Ϊ��������
     * @post m_position += 2 + value.size()
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteStringF16(const std::string& value);

    /**
     * @brief д��std::string���͵�����,��uint32_t��Ϊ��������
     * @post m_position += 4 + value.size()
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteStringF32(const std::string& value);

    /**
     * @brief д��std::string���͵�����,��uint64_t��Ϊ��������
     * @post m_position += 8 + value.size()
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteStringF64(const std::string& value);

    /**
     * @brief д��std::string���͵�����,���޷���Varint64��Ϊ��������
     * @post m_position += Varint64���� + value.size()
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteStringVint(const std::string& value);

    /**
     * @brief д��std::string���͵�����,�޳���
     * @post m_position += value.size()
     *       ���m_position > m_size �� m_size = m_position
     */
    void WriteStringWithoutLength(const std::string& value);

    /**
     * @brief ��ȡint8_t���͵�����
     * @pre getReadSize() >= sizeof(int8_t)
     * @post m_position += sizeof(int8_t);
     * @exception ���getReadSize() < sizeof(int8_t) �׳� std::out_of_range
     */
    int8_t   ReadFint8();

    /**
     * @brief ��ȡuint8_t���͵�����
     * @pre getReadSize() >= sizeof(uint8_t)
     * @post m_position += sizeof(uint8_t);
     * @exception ���getReadSize() < sizeof(uint8_t) �׳� std::out_of_range
     */
    uint8_t  ReadFuint8();

    /**
     * @brief ��ȡint16_t���͵�����
     * @pre getReadSize() >= sizeof(int16_t)
     * @post m_position += sizeof(int16_t);
     * @exception ���getReadSize() < sizeof(int16_t) �׳� std::out_of_range
     */
    int16_t  ReadFint16();

    /**
     * @brief ��ȡuint16_t���͵�����
     * @pre getReadSize() >= sizeof(uint16_t)
     * @post m_position += sizeof(uint16_t);
     * @exception ���getReadSize() < sizeof(uint16_t) �׳� std::out_of_range
     */
    uint16_t ReadFuint16();

    /**
     * @brief ��ȡint32_t���͵�����
     * @pre getReadSize() >= sizeof(int32_t)
     * @post m_position += sizeof(int32_t);
     * @exception ���getReadSize() < sizeof(int32_t) �׳� std::out_of_range
     */
    int32_t  ReadFint32();

    /**
     * @brief ��ȡuint32_t���͵�����
     * @pre getReadSize() >= sizeof(uint32_t)
     * @post m_position += sizeof(uint32_t);
     * @exception ���getReadSize() < sizeof(uint32_t) �׳� std::out_of_range
     */
    uint32_t ReadFuint32();

    /**
     * @brief ��ȡint64_t���͵�����
     * @pre getReadSize() >= sizeof(int64_t)
     * @post m_position += sizeof(int64_t);
     * @exception ���getReadSize() < sizeof(int64_t) �׳� std::out_of_range
     */
    int64_t  ReadFint64();

    /**
     * @brief ��ȡuint64_t���͵�����
     * @pre getReadSize() >= sizeof(uint64_t)
     * @post m_position += sizeof(uint64_t);
     * @exception ���getReadSize() < sizeof(uint64_t) �׳� std::out_of_range
     */
    uint64_t ReadFuint64();

    /**
     * @brief ��ȡ�з���Varint32���͵�����
     * @pre getReadSize() >= �з���Varint32ʵ��ռ���ڴ�
     * @post m_position += �з���Varint32ʵ��ռ���ڴ�
     * @exception ���getReadSize() < �з���Varint32ʵ��ռ���ڴ� �׳� std::out_of_range
     */
    int32_t  ReadInt32();

    /**
     * @brief ��ȡ�޷���Varint32���͵�����
     * @pre getReadSize() >= �޷���Varint32ʵ��ռ���ڴ�
     * @post m_position += �޷���Varint32ʵ��ռ���ڴ�
     * @exception ���getReadSize() < �޷���Varint32ʵ��ռ���ڴ� �׳� std::out_of_range
     */
    uint32_t ReadUint32();

    /**
     * @brief ��ȡ�з���Varint64���͵�����
     * @pre getReadSize() >= �з���Varint64ʵ��ռ���ڴ�
     * @post m_position += �з���Varint64ʵ��ռ���ڴ�
     * @exception ���getReadSize() < �з���Varint64ʵ��ռ���ڴ� �׳� std::out_of_range
     */
    int64_t  ReadInt64();

    /**
     * @brief ��ȡ�޷���Varint64���͵�����
     * @pre getReadSize() >= �޷���Varint64ʵ��ռ���ڴ�
     * @post m_position += �޷���Varint64ʵ��ռ���ڴ�
     * @exception ���getReadSize() < �޷���Varint64ʵ��ռ���ڴ� �׳� std::out_of_range
     */
    uint64_t ReadUint64();

    /**
     * @brief ��ȡfloat���͵�����
     * @pre getReadSize() >= sizeof(float)
     * @post m_position += sizeof(float);
     * @exception ���getReadSize() < sizeof(float) �׳� std::out_of_range
     */
    float    ReadFloat();

    /**
     * @brief ��ȡdouble���͵�����
     * @pre getReadSize() >= sizeof(double)
     * @post m_position += sizeof(double);
     * @exception ���getReadSize() < sizeof(double) �׳� std::out_of_range
     */
    double   ReadDouble();

    /**
     * @brief ��ȡstd::string���͵�����,��uint16_t��Ϊ����
     * @pre getReadSize() >= sizeof(uint16_t) + size
     * @post m_position += sizeof(uint16_t) + size;
     * @exception ���getReadSize() < sizeof(uint16_t) + size �׳� std::out_of_range
     */
    std::string ReadStringF16();

    /**
     * @brief ��ȡstd::string���͵�����,��uint32_t��Ϊ����
     * @pre getReadSize() >= sizeof(uint32_t) + size
     * @post m_position += sizeof(uint32_t) + size;
     * @exception ���getReadSize() < sizeof(uint32_t) + size �׳� std::out_of_range
     */
    std::string ReadStringF32();

    /**
     * @brief ��ȡstd::string���͵�����,��uint64_t��Ϊ����
     * @pre getReadSize() >= sizeof(uint64_t) + size
     * @post m_position += sizeof(uint64_t) + size;
     * @exception ���getReadSize() < sizeof(uint64_t) + size �׳� std::out_of_range
     */
    std::string ReadStringF64();

    /**
     * @brief ��ȡstd::string���͵�����,���޷���Varint64��Ϊ����
     * @pre getReadSize() >= �޷���Varint64ʵ�ʴ�С + size
     * @post m_position += �޷���Varint64ʵ�ʴ�С + size;
     * @exception ���getReadSize() < �޷���Varint64ʵ�ʴ�С + size �׳� std::out_of_range
     */
    std::string ReadStringVint();

private:
    // �ڴ���С
    const size_t node_size_;
    // ͷ�ڵ�
    Node* head_node_;
    // β�ڵ�
    Node* tail_node_;

    // дλ��
    size_t write_pos_;
    // ��λ��
    size_t read_pos_;
    // ������
    size_t capacity_;

    // д�ڵ�
    Node* write_node_;
    // ���ڵ�
    Node* read_node_;

    // �����ֽ���
    int8_t data_endian_;
};

XCO_NAMESPAVE_END
