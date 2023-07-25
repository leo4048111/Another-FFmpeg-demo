//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_THREAD_H
#define DEMO2_THREAD_H
#include <thread>
#include <memory>

class Thread
{
public:
    Thread() {}
    ~Thread()
    {
        Stop();
    }

    virtual int Start()
    {
        thread_->detach();
        return 0;
    }

    virtual int Stop()
    {
        abort_ = 1;
        return 0;
    }
    // 纯虚函数只有函数的名字而不具备函数的功能，不能被调用。
    // 它只是通知编译系统: “在这里声明一个虚函数，留待派生类中定义”。在派生类中对此函数提供定义后，它才能具备函数的功能，可被调用。
    virtual void Run() = 0;

protected:
    std::unique_ptr<std::thread> thread_;
    int abort_ = 0;
};

#endif // DEMO2_THREAD_H
