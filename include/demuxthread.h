//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_DEMUXTHREAD_H
#define DEMO2_DEMUXTHREAD_H
#include "log.h"
#include "queue.h"
#include "thread.h"
#include "avpacketqueue.h"
#include "avframequeue.h"

#ifdef __cplusplus
extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#endif

//继承Thread
class DemuxThread:public Thread{
public:
    DemuxThread(AVPacketQueue* audioqueue, AVPacketQueue* videoqueue);
    ~DemuxThread();
    int Init(const char* url);
    int Start() override;
    int Stop() override;
    void Run() override;

    AVCodecParameters *AudioCodecParameters();
    AVCodecParameters *VideoCodecParameters();

private:
    char err[256]={0};
    std::string url_;
    AVPacketQueue* audio_queue_ =NULL;
    AVPacketQueue* video_queue_ =NULL;
    AVFormatContext* fmt_ctx_=NULL;
    int audio_index_=-1;
    int video_index_=-1;
};

#endif //DEMO2_DEMUXTHREAD_H
