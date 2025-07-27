#include "socket.h"
#include "inetaddr.h"
#include "logger.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

Socket::~Socket(){
    close(sockfd_);
}

void Socket::bindAddress(const InetAddr& localaddr){
    if(bind(sockfd_, (struct sockaddr*)localaddr.getSockAddr(), sizeof(struct sockaddr_in)) < 0){
        LOG_FATAL("bind sockfd:%d fail", sockfd_);
    }
}

void Socket::listen(){
    if(::listen(sockfd_, SOMAXCONN) < 0){
        LOG_FATAL("listen sockfd:%d fail", sockfd_);
    }
}


int Socket::accept(InetAddr* peeraddr){
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    int connfd = ::accept(sockfd_, (struct sockaddr*)&addr, &len);
    if(connfd < 0){
        LOG_FATAL("accept4 sockfd:%d fail", sockfd_);
    }
    if(!peeraddr){
        LOG_ERROR("peeraddr is null");
    }
    peeraddr->setSockAddr(addr);
    return connfd;
}

void Socket::shutdownWrite() {
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        LOG_FATAL("shutdown sockfd:%d fail", sockfd_);
    }
}

void Socket::setTcpNoDelay(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP,  TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}


