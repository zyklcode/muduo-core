#include "inetaddr.h"

InetAddr::InetAddr(uint16_t port, const std::string ip)
    : addr_() {
    memset(&addr_, 0, sizeof(addr_)); // 清空地址结构体
    addr_.sin_family = AF_INET; // 设置地址族为IPv4
    addr_.sin_port = htons(port); // 将端口号转换为网络字节序
    addr_.sin_addr.s_addr = inet_addr(ip.c_str()); // 将IP地址转换为网络字节序
}

InetAddr::InetAddr(const struct sockaddr_in& addr)
    : addr_(addr) {
}

const struct sockaddr_in* InetAddr::getSockAddr() const {
    return &addr_;
}

std::string InetAddr::toIp() const {
    char buffer[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buffer, sizeof(buffer)); // 将网络字节序的IP地址转换为字符串
    return std::string(buffer);
}

uint16_t InetAddr::toPort() const {
    return ntohs(addr_.sin_port); // 将端口号从网络字节序转换为主机字节序
}

std::string InetAddr::toIpPort() const {
    return toIp() + ":" + std::to_string(toPort()); // 将IP地址和端口号转换为字符串形式
}


// #include <iostream>
// int main() {
//     InetAddr addr(8000, "10.32.23.1");
//     std::cout << "IP: " << addr.toIp() << std::endl;
//     std::cout << "Port: " << addr.toPort() << std::endl;
//     std::cout << "IP:Port: " << addr.toIpPort() << std::endl;
//     return 0;
// }