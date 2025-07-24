#pragma once

#include <ctime>
#include <string>

// 时间类
class Timestamp {
public:
    Timestamp();
    explicit Timestamp(time_t secondsSinceEpoch);
    ~Timestamp();
    static Timestamp now();
    std::string toString() const;
private:
    time_t secondsSinceEpoch_;
};