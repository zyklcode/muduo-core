#pragma once

namespace CurrentThread
{
    extern __thread int t_cachedTid; // 当前线程的线程ID

    void cacheTid(); // 缓存线程ID
    inline int tid(){
        if(__builtin_expect(t_cachedTid == 0, 0)){
            cacheTid();
        }
        return t_cachedTid;
    }
}