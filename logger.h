#pragma once

#include "nocopyable.h"

#include <string>
#include <iostream>


#define LOG_INFO(logmsgFormat, ...) do { \
    Logger& logger = Logger::getInstance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::INFO)); \
    char s[1024] = {0}; \
    snprintf(s, 1024, logmsgFormat, ##__VA_ARGS__); \
    logger.log(s); \
} while(0)

#define LOG_ERROR(logmsgFormat,...) do { \
    Logger& logger = Logger::getInstance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::ERROR)); \
    char s[1024] = {0}; \
    snprintf(s, 1024, logmsgFormat, ##__VA_ARGS__); \
    logger.log(s); \
} while(0)

#define LOG_FATAL(logmsgFormat,...) do { \
    Logger& logger = Logger::getInstance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::FATAL)); \
    char s[1024] = {0}; \
    snprintf(s, 1024, logmsgFormat, ##__VA_ARGS__); \
    logger.log(s); \
    exit(-1); \
} while(0)


#ifdef MUDUO_CORE_DEBUG
#define LOG_DEBUG(logmsgFormat,...) do { \
    Logger& logger = Logger::getInstance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::DEBUG)); \
    char s[1024] = {0}; \
    snprintf(s, 1024, logmsgFormat, ##__VA_ARGS__); \
    logger.log(s); \
} while(0)
#else
#define LOG_DEBUG(logmsgFormat,...)
#endif


// 定义日志的级别 INFO ERROR FATAL DEBUG
enum class LogLevel {
    INFO,       // 普通信息
    ERROR,      // 错误信息
    FATAL,      // 致命错误
    DEBUG       // 调试信息
};

// 输出一个日志类，单例模式
class Logger: nocopyable{
public:
    // 获取日志唯一的实例对象
    static Logger& getInstance();
    // 设置日志级别
    void setLogLevel(int level);

    // 写日志
    void log(std::string msg);

private:
    Logger() = default;

    // 定义日志级别
    int logLevel_;
};