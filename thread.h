#pragma once

#include "nocopyable.h"
#include <functional>
#include <thread>
#include <memory>
#include <string>
#include <atomic>

class Thread: nocopyable{
public:
        using ThreadFunc = std::function<void()>;

        explicit Thread(ThreadFunc func, const std::string& name = std::string());
        ~Thread();

        void start();
        void join();

        bool started() const { return started_; }
        bool joined() const { return joined_; }

        pid_t tid() const { return tid_; }

        const std::string& name() const { return name_; }

        static int numCreated() { return numCreated_; }
protected:
    void setDefaultName();

private:
    bool joined_;
    bool started_;
    std::shared_ptr<std::thread> threadPtr_;
    ThreadFunc func_;
    pid_t tid_;
    std::string name_;
    static std::atomic<int> numCreated_;
};