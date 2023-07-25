//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_QUEUE_H
#define DEMO2_QUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
// 模版类，该类的所有函数必须放在头文件中
class Queue
{
public:
    Queue() {}
    ~Queue() {}
    void Abort()
    {
        abort_ = 1;
        cond_.notify_all();
    }

    // lock_guard换成unique_lock是一样的。
    int Push(T val)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (abort_)
            return -1;
        queue_.push(val);

        return 0;
    }

    int Pop(T &val, const int timeout = 0)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::unique_lock<std::mutex> lock2(cond_mutex_);
        // return 如果返回false继续wait，如果返回true就退出wait
        // 即一直wait直到返回true
        if (queue_.empty())
        {
            cond_.wait_for(lock2, std::chrono::milliseconds(timeout), [this]
                           { return !queue_.empty() | abort_; });
        }
        if (abort_ == 1)
        {
            return -1;
        }
        if (queue_.empty())
        {
            return -1;
        }
        val = queue_.front();
        queue_.pop();
        return 0;
    }

    int Front(T &val)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (abort_ == 1)
        {
            return -1;
        }
        if (queue_.empty())
        {
            return -1;
        }
        val = queue_.front();
        return 0;
    }

    int Size()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        int size = queue_.size();
        return size;
    }

private:
    int abort_ = 0;
    std::mutex mutex_;
    std::mutex cond_mutex_;
    std::condition_variable cond_;
    std::queue<T> queue_;
};

#endif // DEMO2_QUEUE_H
