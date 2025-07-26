#include "currentthread.h"
#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread {
    __thread int t_cachedTid = 0; // 初始化线程ID为0

    void cacheTid(){
        if(t_cachedTid == 0){
            // 使用syscall获取当前线程的ID
            // SYS_gettid是Linux系统调用的编号，用于获取线程ID
            t_cachedTid = static_cast<int>(syscall(SYS_gettid));
        }
    }
}