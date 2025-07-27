#include "tcpserver.h"


void* checkNotNull(void* loop) {
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
        , acceptor_(std::make_unique<Acceptor>(mainLoop, addr, option == kReusePort))
        , threadPool_(std::make_shared<EventLoopThreadPool>(mainLoop, name))
        , connectionCallback_()
        , messageCallback_()
        , nextConnId_(1)
{
    // 当有新连接时，会调用TcpServer::newConnection
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
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

void TcpServer::newConnection(int sockfd, const InetAddr& addr){
    
}