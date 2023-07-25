//
// Created by 李茹玥 on 2023/7/13.
//
//由音频帧队列输出声音。将数字信号还原成模拟声音信号输出
#ifndef DEMO2_AUDIOOUTPUT_H
#define DEMO2_AUDIOOUTPUT_H

#ifdef __cplusplus
extern "C"{
#include <SDL2/SDL.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
};

#endif
#include "avframequeue.h"


typedef struct AudioParams
{
    int freq;
    int channels;
    uint64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
}AudioParams;

class AudioOutput{
public:
    AudioOutput(const AudioParams &audio_params, AVFrameQueue* frame_queue);
    ~AudioOutput();
    int Init();
    int DeInit();

public:
    AudioParams src_tgt_;//解码后的参数
    AudioParams dst_tgt_;//SDL实际输出的参数
    AVFrameQueue *frame_queue_=NULL;
    struct SwrContext *swr_cxt_=NULL;
    SDL_AudioSpec spec_;

    uint8_t *audio_buf_=NULL;
    uint8_t *audio_buf1_=NULL;
    uint8_t audio_buf_size=0;
    uint8_t audio_buf1_size=0;
    uint8_t audio_buf_index=0;
};


#endif //DEMO2_AUDIOOUTPUT_H
