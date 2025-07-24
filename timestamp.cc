#include "timestamp.h"
// 默认构造函数
Timestamp::Timestamp() : secondsSinceEpoch_(0) {}

// 显式构造函数
Timestamp::Timestamp(int64_t secondsSinceEpoch)
    : secondsSinceEpoch_(secondsSinceEpoch) {}

// 析构函数
Timestamp::~Timestamp() {}

// 获取当前时间戳
Timestamp Timestamp::now() {
    return Timestamp(time(nullptr));
}

// 将时间戳转换为字符串
std::string Timestamp::toString() const {
    struct tm* time_struct = localtime(&secondsSinceEpoch_);
    char buffer[26] = {0};
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", time_struct);
    return std::string(buffer);
}