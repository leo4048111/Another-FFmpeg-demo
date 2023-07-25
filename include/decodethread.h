//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_DECODE_H
#define DEMO2_DECODE_H

#include "queue.h"
#include "avframequeue.h"
#include "avpacketqueue.h"
#include "thread.h"

class DecodeThread:public Thread
{
public:
    DecodeThread(AVPacketQueue* packet_queue, AVFrameQueue* frame_queue);
    ~DecodeThread();
    int Init(AVCodecParameters* par);
    int Start();
    int Stop();
    void Run();

private:
    char err[256]={0};
    AVPacketQueue* packet_queue_=NULL;
    AVFrameQueue* frame_queue_=NULL;
    AVCodecContext* codec_ctx_=NULL;
};


#endif //DEMO2_DECODE_H
