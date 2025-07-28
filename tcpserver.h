#pragma once
#include "nocopyable.h"
#include "inetaddr.h"
#include "acceptor.h"
#include "eventloop.h"
#include "eventloopthreadpool.h"
#include "callbacks.h"
#include "logger.h"
#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

class TcpServer: public nocopyable{
public:
    enum Option{
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* mainLoop, 
        const InetAddr& addr,
        const std::string& name,
        Option option = kNoReusePort);

    ~TcpServer();

    const std::string& ipPort() const { return ipPort_; }
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return mainLoop_; }

    void setThreadInitCallback(const ThreadInitCallback& cb) {
        threadInitCallback_ = cb;
    }
    // 设置新连接的回调函数
    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    // 设置消息回调函数
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    // 设置写完成回调函数
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    // 设置底层subloop的个数
    void setThreadNum(int numThreads);

    // 启动TcpServer
    void start();

private:
    void newConnection(int sockfd, const InetAddr& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    
    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop* mainLoop_;
    const std::string ipPort_;
    const std::string name_;
    const InetAddr localAddr_;
    std::unique_ptr<Acceptor> acceptor_;            // 运行在mainLoop_, 任务是监听新的连接事件

    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;         // 有新连接时候的回调
    MessageCallback messageCallback_;               // 有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_;   // 消息发送完成时的回调
    
    ThreadInitCallback threadInitCallback_;         // 线程初始化回调

    std::atomic<int> started_;

    int nextConnId_;
    ConnectionMap connections_; // 保存所有连接的map

};