#include "thread.h"
#include "currentthread.h"
#include <semaphore.h>


Thread::Thread(ThreadFunc func, const std::string& name)
: joined_(false)
, started_(false)
, threadPtr_(nullptr)
, tid_(0)
, name_(name)
, func_(std::move(func))
{
    numCreated_++;
    setDefaultName();
}


void Thread::setDefaultName() {
    if (name_.empty()) {
        name_ = "Thread_" + std::to_string(numCreated_);
    }
}

Thread::~Thread() {
    if (started_ && !joined_) {
        threadPtr_->detach(); // 如果线程未被join，则分离线程
    }
}

void Thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem, 0, 0);
    // 开启线程
    threadPtr_ = std::make_shared<std::thread>([&]() {
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_(); // 执行线程函数
    });
    sem_wait(&sem);
}

void Thread::join() {
    if (started_ && !joined_) {
        joined_ = true;
        threadPtr_->join(); // 如果线程未被join，则等待线程结束
    }
}