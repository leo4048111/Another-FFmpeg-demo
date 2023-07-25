//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_AVPACKETQUEUE_H
#define DEMO2_AVPACKETQUEUE_H
#include "queue.h"
#include "log.h"

#include <mutex>

#ifdef __cplusplus
extern "C"{
#include <libavcodec/avcodec.h>
};

#endif

class AVPacketQueue
{
public:
    AVPacketQueue();
    ~AVPacketQueue();
    void Abort();
    int Push(AVPacket *val);
    AVPacket* Pop(const int timeout);
    AVPacket* Front();
    int Size();
private:
    void release();
    Queue<AVPacket*> queue_;
};


#endif //DEMO2_AVPACKETQUEUE_H
