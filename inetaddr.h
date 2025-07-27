#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>

// 封装IP地址和端口号
class InetAddr{
public:
    InetAddr() = default;
    explicit InetAddr(uint16_t port, const std::string ip = "127.0.0.1");
    explicit InetAddr(const struct sockaddr_in& addr);
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;
    const struct sockaddr_in* getSockAddr() const;
    void setSockAddr(const struct sockaddr_in& addr);
private:
    struct sockaddr_in addr_; // IPv4地址结构体
};