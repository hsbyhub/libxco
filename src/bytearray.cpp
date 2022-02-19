/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/13
 *================================================================*/

#include "bytearray.h"
#include <iomanip>
#include <fstream>

#define HIGHT_ONE_BIT_REAL_IN_BYTE (1 << 7)
#define LOW_SEVEN_BIT_REAL_IN_BYTE (HIGHT_ONE_BIT_REAL_IN_BYTE - 1)

XCO_NAMESPAVE_START

ByteArray::Node::Node(size_t s)
    : data(new char[s])
    , cap(s){
}

ByteArray::Node::~Node() {
    if (data) {
        delete[] data;
    }
}

ByteArray::ByteArray(size_t nodesize)
    : node_size_(nodesize)
    , head_node_(new Node(nodesize))
    , tail_node_(head_node_)
    , write_pos_(0)
    , read_pos_(0)
    , capacity_(nodesize)
    , write_node_(head_node_)
    , read_node_(head_node_)
    , data_endian_(XCO_ENDIAN_BIG){
}

ByteArray::~ByteArray() {
    while(head_node_) {
        auto next = head_node_->next;
        delete head_node_;
        head_node_ = next;
    }
}

void ByteArray::Dump(std::ostream &os) const {
    os << "ByteArray{" << EXP_VARS(node_size_, write_pos_, read_pos_, capacity_, data_endian_) << "}";
}

size_t ByteArray::GetCapSize() const{
    return capacity_ - write_pos_;
}

size_t ByteArray::GetDataSize() const{
    return write_pos_ - read_pos_;
}

void ByteArray::FitCapacity(size_t need_cap) {
    size_t left_cap = GetCapSize();
    if (left_cap >= need_cap) {
        return;
    }

    size_t target_cap = capacity_ <= 0 ? 1 : capacity_;
    while(target_cap < need_cap) {
        target_cap <<= 1;
    }

    size_t add_cap = target_cap - left_cap;
    size_t add_node_count = ceil(1.0 * add_cap / node_size_);

    for (size_t i = 0; i < add_node_count; ++i) {
        auto new_node = new Node(node_size_);
        tail_node_->next = new_node;
        tail_node_ = new_node;
        if (write_node_ == nullptr) {
            write_node_ = new_node;
        }
        capacity_ += node_size_;
    }
}

ByteArray::Node *ByteArray::GetNode(size_t pos) const{
    if (pos > capacity_) {
        return nullptr;
    }
    Node* node = head_node_;
    for (size_t i = 0; i < pos / node_size_; ++i) {
        if (!node) {
            return nullptr;
        }
        node = node->next;
    }
    return node;
}

size_t ByteArray::GetNodePos(size_t pos) const {
    return pos % node_size_;
}

size_t ByteArray::Iter(ByteArray::Node **node, size_t* pos, size_t len, size_t node_size, ByteArray::IterOptFuncType opt_func) {
    size_t opt_size = 0;
    while(len > 0) {
        if (!*node) {
            throw std::out_of_range("access out of range");
        }

        size_t node_pos = *pos % node_size;
        size_t node_len = std::min(len, node_size - node_pos);

        // ?????????
        opt_func(*node, node_pos, node_len, opt_size);

        *pos += node_len;
        len -= node_len;
        opt_size += node_len;
        if (node_pos + node_len >= node_size) {
            *node = (*node)->next;
        }
    }
    return opt_size;
}

void ByteArray::Clear() {
    // ???????????
    Node* ptr = head_node_->next;
    while(ptr) {
        auto next = ptr->next;
        delete ptr;
        ptr = next;
    }
    write_node_ = read_node_ = tail_node_ = head_node_;
    head_node_->next = nullptr;
    read_pos_ = write_pos_ = 0;
    capacity_ = node_size_;
}

bool ByteArray::IsLittleEndian() const {
    return data_endian_ == XCO_ENDIAN_LITTLE;
}

void ByteArray::SetIsLittleEndian(bool val) {
    data_endian_ = val ? XCO_ENDIAN_LITTLE : XCO_ENDIAN_BIG;
}

void ByteArray::SetWritePos(size_t pos) {
    if (pos > capacity_) {
        FitCapacity(pos - capacity_);
    }
    write_node_ = GetNode(pos);
    write_pos_= pos;
}

void ByteArray::AddWritePos(size_t count) {
    SetWritePos(write_pos_ + count);
}

void ByteArray::SetReadPos(size_t pos) {
    if (pos > capacity_) {
        throw std::out_of_range("set read pos out of range");
    }
    if (pos > write_pos_) {
        pos = write_pos_;
    }
    read_node_ = GetNode(pos);
    read_pos_ = pos;
}

void ByteArray::AddReadPos(size_t count) {
    SetReadPos(read_pos_ + count);
}

std::string ByteArray::DataToTextString() const{
    auto size = write_pos_;
    std::string res;
    res.resize(size);
    Read(&res[0], res.size(), 0);
    return res;
}

std::string ByteArray::DataToHexString() const {
    std::string str = DataToTextString();
    std::stringstream ss;

    for (size_t i = 0; i < str.size(); ++i) {
        if (i > 0 && i % (1 << 5) == 0) {
            ss << std::endl;
        }
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)str[i] << " ";
    }
    return ss.str();
}


bool ByteArray::WriteToFile(const std::string &name) const {
    std::ofstream ofs(name, std::ios::trunc | std::ios::binary);
    if (!ofs) {
        return false;
    }

    size_t pos = read_pos_;
    Node* node = read_node_;
    Iter(&node, &pos, write_pos_ - pos, GetNodeSize(), [&ofs](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        ofs.write(node->data + node_pos, node_len);
    });
    ofs.close();
    return true;
}

bool ByteArray::ReadFromFile(const std::string &name) {
    std::ifstream ifs(name, std::ios::binary);
    if (!ifs) {
        return false;
    }
    char buf[4096];
    MemSetZero(buf);
    while(!ifs.eof()) {
        ifs.read(buf, node_size_);
        Write(buf, ifs.gcount());
    }
    ifs.close();
    return true;
}

uint64_t ByteArray::GetReadIovecs(std::vector<iovec> &iovs, uint64_t len) const {
    return GetReadIovecs(iovs, len, read_pos_);
}

uint64_t ByteArray::GetReadIovecs(std::vector<iovec> &iovs, uint64_t len, uint64_t read_pos) const {
    if (read_pos >= write_pos_) {
        return 0;
    }

    len = std::min(write_pos_ - read_pos, len);
    if (len <= 0) {
        return 0;
    }

    iovec iov;
    MemSetZero(iov);
    auto node = GetNode(read_pos);
    return Iter(&node, &read_pos, len, GetNodeSize(), [&iovs, &iov](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        iov.iov_base = node->data + node_pos;
        iov.iov_len = node_len;
        iovs.push_back(iov);
    });
}

uint64_t ByteArray::GetWriteIovecs(std::vector<iovec> &iovs, uint64_t len) {
    if (len <= 0) {
        return 0;
    }

    FitCapacity(len);

    iovec iov;
    MemSetZero(iov);
    auto write_pos = write_pos_;
    auto node = GetNode(write_pos);
    return Iter(&node, &write_pos, len, GetNodeSize(), [&iovs, &iov](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        iov.iov_base = node->data + node_pos;
        iov.iov_len = node_len;
        memset(iov.iov_base, 0, iov.iov_len);
        iovs.push_back(iov);
    });
}

void ByteArray::Write(const void *buf, size_t len) {
    if (len <= 0) {
        return ;
    }
    FitCapacity(len);
    Iter(&write_node_, &write_pos_, len, GetNodeSize(), [buf](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        memcpy(node->data + node_pos, (char*)(buf) + opt_size, node_len);
    });
}

size_t ByteArray::Read(void *buf, size_t len) {
    if (len <= 0 || len > GetDataSize()) {
        return 0;
    }

    return Iter(&read_node_, &read_pos_, len, GetNodeSize(), [buf](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        memcpy((char*)(buf) + opt_size, node->data + node_pos, node_len);
    });
}

size_t ByteArray::Read(void *buf, size_t len, size_t pos) const {
    if (len <= 0 || pos + len > write_pos_) {
        return 0;
    }

    auto node = GetNode(pos);
    return Iter(&node, &pos, len, GetNodeSize(), [buf](Node* node, size_t node_pos, size_t node_len, size_t opt_size){
        memcpy((char*)(buf) + opt_size, node->data + node_pos, node_len);
    });
}

void ByteArray::WriteFint8(int8_t value) {
    Write(&value, sizeof(value));
}

void ByteArray::WriteFuint8(uint8_t value) {
    Write(&value, sizeof(value));
}

#define XX                          \
    if (data_endian_ != XCO_ENDIAN) {   \
        value = ByteSwap(value);    \
    }                               \
    Write(&value, sizeof(value))

void ByteArray::WriteFint16(int16_t value) {
    XX;
}

void ByteArray::WriteFuint16(uint16_t value) {
    XX;
}

void ByteArray::WriteFint32(int32_t value) {
    XX;
}

void ByteArray::WriteFuint32(uint32_t value) {
    XX;
}

void ByteArray::WriteFint64(int64_t value) {
    XX;
}

void ByteArray::WriteFuint64(uint64_t value) {
    XX;
}
#undef XX

/**
 * @brief Zigzagλ?????
 * @param[in] v:32λ?з???????
 * @return ?????????????
 */
static uint32_t EncodeZigzag32(int32_t v) {
    return (v >> 31) ^ (v << 1);
}

/**
 * @brief Zigzagλ?????
 * @param[in] v:64λ?з???????
 * @return ?????????????
 */
static uint64_t EncodeZigzag64(int64_t v) {
    return (v >> 63) ^ (v << 1);
}

/**
 * @brief Zigzagλ???????
 * @param[in] v:32λ?????????
 * @return ?з???????????
 */
static int32_t DecodeZigzag32(uint32_t v) {
    return (v >> 1) ^ -(v & 1);
}

/**
 * @brief Zigzagλ???????
 * @param[in] v:64λ?????????
 * @return ?з???????????
 */
static int64_t DecodeZigzag64(uint64_t v) {
    return (v >> 1) ^ -(v & 1);
}

void ByteArray::WriteInt32(int32_t value) {
    WriteUint32(EncodeZigzag32(value));
}

void ByteArray::WriteUint32(uint32_t value) {
    uint8_t bytes[5];
    uint8_t byte_count = 0;
    while(value & ~(uint32_t)LOW_SEVEN_BIT_REAL_IN_BYTE) {
        bytes[byte_count++] = (value & LOW_SEVEN_BIT_REAL_IN_BYTE) | HIGHT_ONE_BIT_REAL_IN_BYTE;
        value >>= 7;
    }
    bytes[byte_count++] = value;
    Write(bytes, byte_count);
}

void ByteArray::WriteInt64(int64_t value) {
    WriteUint64(EncodeZigzag64(value));
}

void ByteArray::WriteUint64(uint64_t value) {
    uint8_t bytes[10];
    uint8_t byte_count = 0;
    while(value & ~(uint64_t)LOW_SEVEN_BIT_REAL_IN_BYTE) {
        bytes[byte_count++] = (value & LOW_SEVEN_BIT_REAL_IN_BYTE) | HIGHT_ONE_BIT_REAL_IN_BYTE;
        value >>= 7;
    }
    bytes[byte_count++] = value;
    Write(bytes, byte_count);
}

void ByteArray::WriteFloat(float value) {
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    WriteFuint32(v);
}

void ByteArray::WriteDouble(double value) {
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    WriteFuint64(v);
}

void ByteArray::WriteStringF16(const std::string &value) {
    WriteFuint16(value.size());
    Write(value.c_str(), value.size());
}

void ByteArray::WriteStringF32(const std::string &value) {
    WriteFuint32(value.size());
    Write(value.c_str(), value.size());
}

void ByteArray::WriteStringF64(const std::string &value) {
    WriteFuint64(value.size());
    Write(value.c_str(), value.size());
}

void ByteArray::WriteStringVint(const std::string &value) {
    WriteUint64(value.size());
    Write(value.c_str(), value.size());
}

void ByteArray::WriteStringWithoutLength(const std::string &value) {

}

#define XX(type)                    \
    type value;                     \
    Read(&value, sizeof(value));    \
    if (data_endian_ != XCO_ENDIAN) {   \
        return ByteSwap(value);     \
    }                               \
    return value

int8_t ByteArray::ReadFint8() {
    int8_t value;
    Read(&value, sizeof(value));
    return value;
}

uint8_t ByteArray::ReadFuint8() {
    uint8_t value;
    Read(&value, sizeof(value));
    return value;
}


int16_t ByteArray::ReadFint16() {
    XX(int16_t);
}

uint16_t ByteArray::ReadFuint16() {
    XX(uint16_t);
}

int32_t ByteArray::ReadFint32() {
    XX(int32_t);
}

uint32_t ByteArray::ReadFuint32() {
    XX(uint32_t);
}

int64_t ByteArray::ReadFint64() {
    XX(int64_t);
}

uint64_t ByteArray::ReadFuint64() {
    XX(uint64_t);
}
#undef XX

int32_t ByteArray::ReadInt32() {
    return DecodeZigzag32(ReadUint32());
}

uint32_t ByteArray::ReadUint32() {
    uint32_t res = 0;
    for (int i = 0; i < 5; ++i) {
        uint8_t byte = ReadFuint8();
        res |= (uint32_t)(byte & LOW_SEVEN_BIT_REAL_IN_BYTE) << (i * 7);
        if (!(byte & HIGHT_ONE_BIT_REAL_IN_BYTE)) {
            break;
        }
    }
    return res;
}

int64_t ByteArray::ReadInt64() {
    return DecodeZigzag64(ReadUint64());
}

uint64_t ByteArray::ReadUint64() {
    uint64_t res = 0;
    for (int i = 0; i < 10; ++i) {
        uint8_t byte = ReadFuint8();
        res |= ((uint64_t)byte & LOW_SEVEN_BIT_REAL_IN_BYTE) << (i * 7);
        if (!((uint64_t)byte & HIGHT_ONE_BIT_REAL_IN_BYTE)) {
            break;
        }
    }
    return res;
}

float ByteArray::ReadFloat() {
    uint32_t tmp = ReadUint32();
    float value;
    memcpy(&value, &tmp, sizeof(tmp));
    return value;
}

double ByteArray::ReadDouble() {
    uint64_t tmp = ReadUint64();
    double value;
    memcpy(&value, &tmp, sizeof(tmp));
    return value;
}

std::string ByteArray::ReadStringF16() {
    uint16_t len = ReadFuint16();
    std::string buf;
    buf.resize(len);
    Read(&buf[0], len);
    return buf;
}

std::string ByteArray::ReadStringF32() {
    uint32_t len = ReadFuint32();
    std::string buf;
    buf.resize(len);
    Read(&buf[0], len);
    return buf;
}

std::string ByteArray::ReadStringF64() {
    uint64_t len = ReadFuint64();
    std::string buf;
    buf.resize(len);
    Read(&buf[0], len);
    return buf;
}

std::string ByteArray::ReadStringVint() {
    uint64_t len = ReadUint64();
    std::string buf;
    buf.resize(len);
    Read(&buf[0], len);
    return buf;
}

XCO_NAMESPAVE_END
