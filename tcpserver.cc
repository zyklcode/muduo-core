#include "tcpserver.h"
#include "eventloopthreadpool.h"
#include "tcpconnection.h"
#include "callbacks.h"


static void* checkNotNull(void* loop) {
    if (loop == nullptr) {
        LOG_FATAL("%s:%s:%d null pointer", __FILE__, __func__, __LINE__);
    }
    return loop;
}


TcpServer::TcpServer(EventLoop* mainLoop, 
        const InetAddr& addr,
        const std::string& name,
        Option option)
        : mainLoop_((EventLoop*)checkNotNull(mainLoop))
        , ipPort_(addr.toIpPort())
        , name_(name)
        , localAddr_(addr)
        , acceptor_(std::make_unique<Acceptor>(mainLoop, addr, option == kReusePort))
        , threadPool_(std::make_shared<EventLoopThreadPool>(mainLoop, name))
        , connectionCallback_()
        , messageCallback_()
        , nextConnId_(1)
{
    // 当有新连接时，会调用TcpServer::newConnection
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){
    for(auto &item: connections_){
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        // 销毁连接
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
        );
    }
}

void TcpServer::setThreadNum(int num){
    threadPool_->setThreadNum(num);
}


// 开启服务器, 然后调用mainLoop_的loop操作
void TcpServer::start(){
    if(started_++ == 0){
        threadPool_->start(threadInitCallback_);
        mainLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}


// 有一个新的客户端的链接，acceptor会执行这个回调操作
void TcpServer::newConnection(int sockfd, const InetAddr& peerAddr){
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s%%%d", ipPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    LOG_INFO("TcpServer::newConnection[%s] from %s", connName.data(), peerAddr.toIpPort().data());
    TcpConnectionPtr conn(std::make_shared<TcpConnection>(ioLoop, connName, sockfd, localAddr_, peerAddr));
    connections_[connName] = conn;
    // 下面的回调都是用户设置给TcpServer=>TcpConnection=>Channel=>Poller=>notify channel调用回调
    conn->setConnectionCallback(connectionCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)
    );
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    mainLoop_->runInLoop(
        std::bind(&TcpServer::removeConnection, this, conn)
    );
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn){
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s", name_.c_str(), conn->name().c_str());

    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}