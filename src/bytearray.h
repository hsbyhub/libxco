/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：xiesenxin
      	创建日期：2022/2/13
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
     * @brief 返回内存块的大小
     */
    size_t GetNodeSize() const { return node_size_;}

    /**
     * @brief 获取的可写容量
     */
    size_t GetCapSize() const;

    /**
     * @brief 获取可读的数据数量
     */
    size_t GetDataSize() const;

    /**
     * @brief 适配容量, 如果剩余容量大于需要的容量，无动作
     * @param[in] need_cap:需要的容量
     */
    void FitCapacity(size_t need_cap);

    Node* GetNode(size_t pos) const;

    size_t GetNodePos(size_t pos) const;

    /**
     * @brief 遍历
     * @param[in]
     * @return
     */
    static size_t Iter(Node** node, size_t* pos, size_t len, size_t node_size, IterOptFuncType opt_func);
public:

    /**
     * @brief 清空ByteArray
     * @post m_position = 0, m_size = 0
     */
    void Clear();

    /**
     * @brief 是否是小端
     */
    bool IsLittleEndian() const;

    /**
     * @brief 设置是否为小端
     */
    void SetIsLittleEndian(bool val);

    /**
     * @brief 获取当前写位置
     */
    size_t GetWritePos() const { return write_pos_;}

    /**
     * @brief 设置当前写位置
     */
    void SetWritePos(size_t pos);

    void AddWritePos(size_t count);

    /**
     * @brief 获取当前读位置
     */
    size_t GetReadPos() const { return read_pos_;}

    /**
     * @brief 设置当前读位置
     */
    void SetReadPos(size_t pos);

    /**
     * @brief 增加当前读位置
     */
    void AddReadPos(size_t count);

public:
    /**
     * @brief 输出所有数据
     */
    std::string DataToTextString() const;

    /**
     * @brief 将ByteArray里面的数据[m_position, m_size)转成16进制的std::string(格式:FF FF FF)
     */
    std::string DataToHexString() const;

    /**
     * @brief 把ByteArray的数据写入到文件中
     * @param[in] name 文件名
     */
    bool WriteToFile(const std::string& name) const;

    /**
     * @brief 从文件中读取数据
     * @param[in] name 文件名
     */
    bool ReadFromFile(const std::string& name);

    /**
     * @brief 获取可读取的缓存,保存成iovec数组
     * @param[out] iovs 保存可读取数据的iovec数组
     * @param[in] len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * @return 返回实际数据的长度
     */
    uint64_t GetReadIovecs(std::vector<iovec>& iovs, uint64_t len = ~0ull) const;

    /**
     * @brief 获取可读取的缓存,保存成iovec数组,从position位置开始
     * @param[out] buffers 保存可读取数据的iovec数组
     * @param[in] len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * @param[in] read_pos 读取数据的位置
     * @return 返回实际数据的长度
     */
    uint64_t GetReadIovecs(std::vector<iovec>& buffers, uint64_t len, uint64_t read_pos) const;

    /**
     * @brief 获取可写入的缓存,保存成iovec数组
     * @param[out] buffers 保存可写入的内存的iovec数组
     * @param[in] len 写入的长度
     * @return 返回实际的长度
     * @post 如果(m_position + len) > m_capacity 则 m_capacity扩容N个节点以容纳len长度
     */
    uint64_t GetWriteIovecs(std::vector<iovec>& buffers, uint64_t len);

    /**
     * @brief 写
     * @param[in] buf:写入的数据
     * @param[in] len:写入的数据大小
     * @return 实际写入的长度
     */
    void Write(const void* buf, size_t len);

    /**
     * @brief 读
     * @param[in] buf:读出的数据
     * @param[in] len:读出的数据大小
     * @return 实际读出的大小
     */
    size_t Read(void* buf, size_t len);

    /**
     * @brief 从pos开始读(不改变读位置)
     * @param[in] buf:读出的数据
     * @param[in] len:读出的数据大小
     * @return 实际读出的大小
     */
    size_t Read(void* buf, size_t len, size_t pos) const;

    /**
     * @brief 写入固定长度int8_t类型的数据
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFint8  (int8_t value);
    /**
     * @brief 写入固定长度uint8_t类型的数据
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFuint8 (uint8_t value);
    /**
     * @brief 写入固定长度int16_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFint16 (int16_t value);
    /**
     * @brief 写入固定长度uint16_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFuint16(uint16_t value);

    /**
     * @brief 写入固定长度int32_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFint32 (int32_t value);

    /**
     * @brief 写入固定长度uint32_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFuint32(uint32_t value);

    /**
     * @brief 写入固定长度int64_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFint64 (int64_t value);

    /**
     * @brief 写入固定长度uint64_t类型的数据(大端/小端)
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFuint64(uint64_t value);

    /**
     * @brief 写入有符号Varint32类型的数据
     * @post m_position += 实际占用内存(1 ~ 5)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteInt32  (int32_t value);
    /**
     * @brief 写入无符号Varint32类型的数据
     * @post m_position += 实际占用内存(1 ~ 5)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteUint32 (uint32_t value);

    /**
     * @brief 写入有符号Varint64类型的数据
     * @post m_position += 实际占用内存(1 ~ 10)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteInt64  (int64_t value);

    /**
     * @brief 写入无符号Varint64类型的数据
     * @post m_position += 实际占用内存(1 ~ 10)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteUint64 (uint64_t value);

    /**
     * @brief 写入float类型的数据
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteFloat  (float value);

    /**
     * @brief 写入double类型的数据
     * @post m_position += sizeof(value)
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteDouble (double value);

    /**
     * @brief 写入std::string类型的数据,用uint16_t作为长度类型
     * @post m_position += 2 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteStringF16(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用uint32_t作为长度类型
     * @post m_position += 4 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteStringF32(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用uint64_t作为长度类型
     * @post m_position += 8 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteStringF64(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用无符号Varint64作为长度类型
     * @post m_position += Varint64长度 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteStringVint(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,无长度
     * @post m_position += value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void WriteStringWithoutLength(const std::string& value);

    /**
     * @brief 读取int8_t类型的数据
     * @pre getReadSize() >= sizeof(int8_t)
     * @post m_position += sizeof(int8_t);
     * @exception 如果getReadSize() < sizeof(int8_t) 抛出 std::out_of_range
     */
    int8_t   ReadFint8();

    /**
     * @brief 读取uint8_t类型的数据
     * @pre getReadSize() >= sizeof(uint8_t)
     * @post m_position += sizeof(uint8_t);
     * @exception 如果getReadSize() < sizeof(uint8_t) 抛出 std::out_of_range
     */
    uint8_t  ReadFuint8();

    /**
     * @brief 读取int16_t类型的数据
     * @pre getReadSize() >= sizeof(int16_t)
     * @post m_position += sizeof(int16_t);
     * @exception 如果getReadSize() < sizeof(int16_t) 抛出 std::out_of_range
     */
    int16_t  ReadFint16();

    /**
     * @brief 读取uint16_t类型的数据
     * @pre getReadSize() >= sizeof(uint16_t)
     * @post m_position += sizeof(uint16_t);
     * @exception 如果getReadSize() < sizeof(uint16_t) 抛出 std::out_of_range
     */
    uint16_t ReadFuint16();

    /**
     * @brief 读取int32_t类型的数据
     * @pre getReadSize() >= sizeof(int32_t)
     * @post m_position += sizeof(int32_t);
     * @exception 如果getReadSize() < sizeof(int32_t) 抛出 std::out_of_range
     */
    int32_t  ReadFint32();

    /**
     * @brief 读取uint32_t类型的数据
     * @pre getReadSize() >= sizeof(uint32_t)
     * @post m_position += sizeof(uint32_t);
     * @exception 如果getReadSize() < sizeof(uint32_t) 抛出 std::out_of_range
     */
    uint32_t ReadFuint32();

    /**
     * @brief 读取int64_t类型的数据
     * @pre getReadSize() >= sizeof(int64_t)
     * @post m_position += sizeof(int64_t);
     * @exception 如果getReadSize() < sizeof(int64_t) 抛出 std::out_of_range
     */
    int64_t  ReadFint64();

    /**
     * @brief 读取uint64_t类型的数据
     * @pre getReadSize() >= sizeof(uint64_t)
     * @post m_position += sizeof(uint64_t);
     * @exception 如果getReadSize() < sizeof(uint64_t) 抛出 std::out_of_range
     */
    uint64_t ReadFuint64();

    /**
     * @brief 读取有符号Varint32类型的数据
     * @pre getReadSize() >= 有符号Varint32实际占用内存
     * @post m_position += 有符号Varint32实际占用内存
     * @exception 如果getReadSize() < 有符号Varint32实际占用内存 抛出 std::out_of_range
     */
    int32_t  ReadInt32();

    /**
     * @brief 读取无符号Varint32类型的数据
     * @pre getReadSize() >= 无符号Varint32实际占用内存
     * @post m_position += 无符号Varint32实际占用内存
     * @exception 如果getReadSize() < 无符号Varint32实际占用内存 抛出 std::out_of_range
     */
    uint32_t ReadUint32();

    /**
     * @brief 读取有符号Varint64类型的数据
     * @pre getReadSize() >= 有符号Varint64实际占用内存
     * @post m_position += 有符号Varint64实际占用内存
     * @exception 如果getReadSize() < 有符号Varint64实际占用内存 抛出 std::out_of_range
     */
    int64_t  ReadInt64();

    /**
     * @brief 读取无符号Varint64类型的数据
     * @pre getReadSize() >= 无符号Varint64实际占用内存
     * @post m_position += 无符号Varint64实际占用内存
     * @exception 如果getReadSize() < 无符号Varint64实际占用内存 抛出 std::out_of_range
     */
    uint64_t ReadUint64();

    /**
     * @brief 读取float类型的数据
     * @pre getReadSize() >= sizeof(float)
     * @post m_position += sizeof(float);
     * @exception 如果getReadSize() < sizeof(float) 抛出 std::out_of_range
     */
    float    ReadFloat();

    /**
     * @brief 读取double类型的数据
     * @pre getReadSize() >= sizeof(double)
     * @post m_position += sizeof(double);
     * @exception 如果getReadSize() < sizeof(double) 抛出 std::out_of_range
     */
    double   ReadDouble();

    /**
     * @brief 读取std::string类型的数据,用uint16_t作为长度
     * @pre getReadSize() >= sizeof(uint16_t) + size
     * @post m_position += sizeof(uint16_t) + size;
     * @exception 如果getReadSize() < sizeof(uint16_t) + size 抛出 std::out_of_range
     */
    std::string ReadStringF16();

    /**
     * @brief 读取std::string类型的数据,用uint32_t作为长度
     * @pre getReadSize() >= sizeof(uint32_t) + size
     * @post m_position += sizeof(uint32_t) + size;
     * @exception 如果getReadSize() < sizeof(uint32_t) + size 抛出 std::out_of_range
     */
    std::string ReadStringF32();

    /**
     * @brief 读取std::string类型的数据,用uint64_t作为长度
     * @pre getReadSize() >= sizeof(uint64_t) + size
     * @post m_position += sizeof(uint64_t) + size;
     * @exception 如果getReadSize() < sizeof(uint64_t) + size 抛出 std::out_of_range
     */
    std::string ReadStringF64();

    /**
     * @brief 读取std::string类型的数据,用无符号Varint64作为长度
     * @pre getReadSize() >= 无符号Varint64实际大小 + size
     * @post m_position += 无符号Varint64实际大小 + size;
     * @exception 如果getReadSize() < 无符号Varint64实际大小 + size 抛出 std::out_of_range
     */
    std::string ReadStringVint();

private:
    // 内存块大小
    const size_t node_size_;
    // 头节点
    Node* head_node_;
    // 尾节点
    Node* tail_node_;

    // 写位置
    size_t write_pos_;
    // 读位置
    size_t read_pos_;
    // 总容量
    size_t capacity_;

    // 写节点
    Node* write_node_;
    // 读节点
    Node* read_node_;

    // 数据字节序
    int8_t data_endian_;
};

XCO_NAMESPAVE_END
