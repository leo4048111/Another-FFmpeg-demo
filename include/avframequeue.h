//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_AVFRAMEQUEUE_H
#define DEMO2_AVFRAMEQUEUE_H

#include "log.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#ifdef __cplusplus
extern "C"{
#include <libavcodec/avcodec.h>
};

#endif

class AVFrameQueue
{
public:
    AVFrameQueue();
    ~AVFrameQueue();
    void Abort();
    int Push(AVFrame *val);
    AVFrame* Pop(const int timeout);
    AVFrame* Front();
    int Size();
private:
    void release();
    Queue<AVFrame*> queue_;
};

#endif //DEMO2_AVFRAMEQUEUE_H
