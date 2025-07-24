#pragma once

/*
* nocopyable被继承后，派生类对象可以构造和析构，但是派生类对象无法进行拷贝和复制操作
*/

class nocopyable{
public:
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;
protected:
    nocopyable() = default;
    ~nocopyable() = default;
};