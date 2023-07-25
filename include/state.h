#pragma once

#ifdef __cplusplus
extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};
#endif

#include "avframequeue.h"

class State
{
public:
    static State& GetInstance() {
        static State instance;
        return instance;
    }

    AVRational video_stream_tb;

    AVFrameQueue* audio_frame_queue;
    AVSampleFormat audio_sample_fmt;
    uint8_t *audio_buf_ = NULL;
    size_t audio_buf_size=0;
    size_t audio_buf_index=0;

    double audio_clock=0;
    double audio_start_time = 0;
};