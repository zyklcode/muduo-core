#pragma once

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;
class Timestamp;
class EventLoop;

using ThreadInitCallback = std::function<void(EventLoop*)>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionEstablishedCallback = std::function<void(TcpConnectionPtr)>;
using ConnectionDestroyedCallback = std::function<void(TcpConnectionPtr)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>;
using WriteCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
