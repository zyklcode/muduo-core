#pragma once
#include <vector>
#include <string>
#include <algorithm>

// 网络库底层的缓冲区定义
class Buffer{
public:
    // 可以记录包的长度
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
    : buffer_(kCheapPrepend + kInitialSize, 0)
    , readIndex_(kCheapPrepend)
    , writeIndex_(kCheapPrepend)
    {}

    size_t readableBytes() const { return writeIndex_ - readIndex_; } // 可读字节数
    size_t writableBytes() const { return buffer_.size() - writeIndex_; } // 可写字节数
    size_t prependableBytes() const { return readIndex_; } // 可预留字节数

    // 返回缓冲区中可读数据的起始地址
    const char* peek() const {
        return cbegin() + readIndex_;
    }

    // onMessage string <- buffer
    void retrieve(size_t len){
        if (len < readableBytes()) {
            readIndex_ += len;
        } else {
            retrieveAll();
        }
        // 如果读完了，就重置索引
        if (readableBytes() == 0) {
            readIndex_ = kCheapPrepend;
            writeIndex_ = kCheapPrepend;
        }
    }

    void retrieveAll(){
        readIndex_ = kCheapPrepend;
        writeIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString(){
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len){
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len){
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    void makeSpace(size_t len){
        /*
        * 
        */
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writeIndex_ + len);
        }else{
            size_t readable = readableBytes();
            std::copy(begin()+readable, begin()+writeIndex_, begin()+kCheapPrepend);
            readIndex_ = kCheapPrepend;
            writeIndex_ = kCheapPrepend + readable;
        }
    }

    void append(const char* data, size_t len){
        ensureWritableBytes(len);
        std::copy(data, data + len, begin() + writeIndex_);
        writeIndex_ += len;
    }

    // 从fd上读取数据
    ssize_t readFd(int fd, int* savedErrno);

    // 从fd上写入数据
    ssize_t writeFd(int fd, int* savedErrno);

private:
    char* begin(){
        return buffer_.data();
    }

    const char* cbegin() const {
        return buffer_.data();
    }

    std::vector<char> buffer_;
    size_t readIndex_ = 0; // 读索引
    size_t writeIndex_ = 0; // 写索引

};