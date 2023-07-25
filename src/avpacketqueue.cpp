//
// Created by 李茹玥 on 2023/7/12.
//
#include "avpacketqueue.h"
#include <string>

AVPacketQueue::AVPacketQueue() {

}

AVPacketQueue::~AVPacketQueue() {

}

void AVPacketQueue::Abort() {
    release();
    queue_.Abort();
}


int AVPacketQueue::Push(AVPacket *val) {
    AVPacket* pkt=av_packet_alloc();
    av_packet_ref(pkt,val);
    int ret = queue_.Push(pkt);
    return ret;
}

AVPacket *AVPacketQueue::Pop(const int timeout) {
    AVPacket* pkt=NULL;
    int res=queue_.Pop(pkt,timeout);
    if(res==-1){
        Serialize("AVPacketQueue::Pop failed");
    }
    return pkt;
}

int AVPacketQueue::Size() {
    return queue_.Size();
}

AVPacket *AVPacketQueue::Front() {
    AVPacket* pkt=NULL;
    int res=queue_.Front(pkt);
    if(res<0){
        Serialize("AVPacketQueue::Front failed");
    }
    return pkt;
}

void AVPacketQueue::release() {
    while(true){
        AVPacket* pkt=NULL;
        int res=queue_.Pop(pkt,1);
        if(res<0){
            break;
        }
        else{
            av_packet_free(&pkt);
        }
    }
}




