#pragma once
#include "nocopyable.h"
#include "callbacks.h"
#include "inetaddr.h"
#include "buffer.h"
#include "timestamp.h"
#include <memory>
#include <string>
#include <atomic>


class Channel;
class EventLoop;
class Socket;


class TcpConnection: nocopyable, public std::enable_shared_from_this<TcpConnection>{
public:
    enum StateE{kDisconnected, kConnecting, kConnected, kDisconnecting};
    TcpConnection(EventLoop* loop_,
                const std::string &name,
                int sockfd,
                const InetAddr &localAddr,
                const InetAddr &peerlocal);
    ~TcpConnection();

    EventLoop* getLoop() const {return loop_;}
    const std::string name() const { return name_; }

    const InetAddr& localAddr() const { return localAddr_;}

    const InetAddr& peerAddr() const {return peerAddr_;}

    bool connected(){ return state_ == kConnected;}
    

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    void setCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    void setState(int state);

    // 连接建立
    void connectEstablished();
    // 连接销毁
    void connectDestroyed();

    // 关闭连接
    void shutdown();

    // 发送数据
    void send(const std::string& buf);


private:
    // 处理读事件
    void handleRead(Timestamp receiveTime);

    // 处理写事件
    void handleWrite();

    // 处理关闭事件
    void handleClose();

    // 处理错误事件
    void handleError();

    
    void sendInLoop(const void* data, size_t len);
    
    void shutdownInLoop();

    EventLoop *loop_;       // 这是的loop_是subLoop，因为客户端fd由subLoop负责监听
    const std::string name_;
    std::atomic<int> state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddr localAddr_;
    const InetAddr peerAddr_;

    ConnectionCallback connectionCallback_;         // 有新连接时候的回调
    MessageCallback messageCallback_;               // 有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_;   // 消息发送完成时的回调
    CloseCallback closeCallback_;                   // 连接关闭时候的回调
    HighWaterMarkCallback highWaterMarkCallback_;   // 高水位线时候的回调
    size_t highWaterMark_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

};