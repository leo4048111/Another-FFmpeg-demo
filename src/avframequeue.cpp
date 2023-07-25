//
// Created by 李茹玥 on 2023/7/12.
//
#include "avframequeue.h"
#include "log.h"

AVFrameQueue::AVFrameQueue() {

}

AVFrameQueue::~AVFrameQueue() {

}

void AVFrameQueue::Abort() {
    release();
    queue_.Abort();
}

int AVFrameQueue::Push(AVFrame *val) {
    AVFrame * frame=av_frame_alloc();
    //视频帧的拷贝
    av_frame_ref(frame,val);
    return queue_.Push(frame);
}

AVFrame *AVFrameQueue::Pop(const int timeout) {
    AVFrame* frame=NULL;
    int res=queue_.Pop(frame,timeout);
    if(res==-1){
        Serialize("AVFrameQueue::Pop failed");
    }
    return frame;
}

AVFrame *AVFrameQueue::Front() {
    AVFrame* frame=NULL;
    int res=queue_.Front(frame);
    if(res<0){
        Serialize("AVFrameQueue::Front failed");
    }
    return frame;
}

int AVFrameQueue::Size() {
    int size = queue_.Size();
    return size;
}

void AVFrameQueue::release() {
    while(true){
        AVFrame * frame=NULL;
        int res=queue_.Pop(frame, 1);
        if(res<0){
            break;
        }
        else{
            av_frame_free(&frame);
        }
    }
}