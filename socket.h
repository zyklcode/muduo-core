#pragma once
#include "nocopyable.h"

class InetAddr;

class Socket: nocopyable{
public:
    explicit Socket(int sockfd): sockfd_(sockfd){}
    ~Socket();

    int fd() const { return sockfd_; }

    void bindAddress(const InetAddr& localaddr);
    void listen();
    int accept(InetAddr* peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};