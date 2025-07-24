#include "poller.h"
// #include "pollpoller.h"
#include "epollpoller.h"
#include <cstdlib>
Poller* Poller::newDefaultPoller(EventLoop *loop){
    if(getenv("MUDUO_CORE_USE_POLL")){
        return nullptr;
    }else{
        return nullptr;
    }
}
