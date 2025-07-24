#include "logger.h"
#include "timestamp.h"

// 获取日志唯一的实例对象
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// 设置日志级别
void Logger::setLogLevel(int level) {
    logLevel_ = level;
}


// 写日志 [界别信息] time : msg
void Logger::log(std::string msg) {
    // 输出日志级别
    switch (logLevel_) {
        case static_cast<int>(LogLevel::INFO):
            std::cout << "[ INFO ]";
            break;
        case static_cast<int>(LogLevel::ERROR):
            std::cout << "[ ERROR]";
            break;
        case static_cast<int>(LogLevel::FATAL):
            std::cout << "[ FATAL]";
            break;
        case static_cast<int>(LogLevel::DEBUG):
            std::cout << "[ DEBUG]";
            break;
        default:
            ;
    }
    // 打印时间和日志消息
    std::cout << " " << Timestamp::now().toString() << " : " << msg << std::endl;
}

int main(int argc, char* argv[]) {
    LOG_DEBUG("hello %s", "world");
    LOG_INFO("hello %s", "world");
    LOG_ERROR("hello %s", "world");
    LOG_FATAL("hello %s", "world");
}