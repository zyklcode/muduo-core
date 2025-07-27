#include "acceptor.h"
#include "logger.h"
#include "inetaddr.h"
#include <sys/socket.h>
#include <unistd.h>


static int createNonblocking(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) {
        LOG_FATAL("create nonblocking socket failed");
    }
    return sockfd;
}


Acceptor::Acceptor(EventLoop* loop, const InetAddr& listenAddr, bool reuseport)
: loop_(loop)
, acceptSocket_(createNonblocking())
, acceptChannel_(loop, acceptSocket_.fd())
, listening_(false)
{
    acceptSocket_.setReuseAddr(reuseport);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor(){
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}


void Acceptor::listen(){
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}
    

// listenfd有事件发生了，就是有新用户连接了
void Acceptor::handleRead(){
    InetAddr peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        if(newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        }
        else {
            ::close(connfd); // 如果没有回调函数，就关闭连接
        }
    }
    else {
        LOG_ERROR("accept error");
    }
}