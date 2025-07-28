#include <muduo_core/tcpserver.h>
#include <muduo_core/tcpconnection.h>
#include <string>
#include <functional>

class EchoServer{
public:
    EchoServer(EventLoop* loop, const InetAddr& addr, const std::string& name)
    : loop_(loop)
    , server_(loop_, addr, name)
    {
        // 注册回调函数
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // 设置合适的线程数
        server_.setThreadNum(4);
    }

    void start(){
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            LOG_INFO("onConnection: %s", conn->peerAddr().toIpPort().c_str());
        }
    }

    // 设置可读写事件回调
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time){
        std::string msg = buffer->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();   // 写段关闭
    }

    TcpServer server_;
    EventLoop *loop_;
};

int main(){
    EventLoop loop;
    InetAddr addr(8888);
    EchoServer server(&loop, addr, "EchoServer");
    server.start();
    loop.loop();
    return 0;
}