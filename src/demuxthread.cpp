//
// Created by 李茹玥 on 2023/7/12.
//
#include "demuxthread.h"
#include "log.h"
#include "state.h"

DemuxThread::DemuxThread(AVPacketQueue *audioqueue, AVPacketQueue *videoqueue)
{
    audio_queue_ = audioqueue;
    video_queue_ = videoqueue;
    Serialize("DemuxThread");
}

DemuxThread::~DemuxThread()
{
    Serialize("~DemuxThread");
}

int DemuxThread::Init(const char *url)
{
    Serialize(url);
    url_ = url;

    fmt_ctx_ = avformat_alloc_context();

    int res = avformat_open_input(&fmt_ctx_, url_.c_str(), NULL, NULL);
    if (res < 0)
    {
        av_strerror(res, err, sizeof(err));
        Serialize("avformat_open_input failed, ret:%d. err:%s", res, err);
        return -1;
    }
    av_dump_format(fmt_ctx_, 0, url_.c_str(), 0);

    audio_index_ = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    video_index_ = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (audio_index_ < 0 || video_index_ < 0)
    {
        Serialize("no audio or no video");
        return -1;
    }
    State::GetInstance().video_stream_tb = fmt_ctx_->streams[video_index_]->time_base;
    auto audio_sample_fmt = (AVSampleFormat)fmt_ctx_->streams[audio_index_]->codecpar->format;
    State::GetInstance().audio_sample_fmt = audio_sample_fmt;
    Serialize("audio_index:%d;video_index:%d", audio_index_, video_index_);
    Serialize("Init leave");
    return 0;
}

int DemuxThread::Start()
{
    thread_ = std::make_unique<std::thread>(&DemuxThread::Run, this);
    thread_->detach();
    return 0;
}

int DemuxThread::Stop()
{
    abort_ = true;
    avformat_close_input(&fmt_ctx_);
    return 0;
}

void DemuxThread::Run()
{
    Serialize("Run");
    int ret = 0;
    AVPacket pkt;

    while (!abort_)
    {
        ret = av_read_frame(fmt_ctx_, &pkt);
        if (ret < 0)
        {
            av_strerror(ret, err, sizeof(err));
            Serialize("av_read_frame(fmt_ctx_,pkt) failed");
            break;
        }

        if (pkt.stream_index == audio_index_)
        {
            ret = audio_queue_->Push(&pkt);
            // Serialize("audio packet size %d", audio_queue_->Size());
        }
        else if (pkt.stream_index == video_index_)
        {
            ret = video_queue_->Push(&pkt);
            // Serialize("video packet size %d", video_queue_->Size());
        }
        av_packet_unref(&pkt);
    }

    Serialize("Run finish");
    return;
}

AVCodecParameters *DemuxThread::VideoCodecParameters()
{
    if (video_index_ != -1)
    {
        return fmt_ctx_->streams[video_index_]->codecpar;
    }
    return NULL;
}

AVCodecParameters *DemuxThread::AudioCodecParameters()
{
    if (audio_index_ != -1)
    {
        return fmt_ctx_->streams[audio_index_]->codecpar;
    }
    return NULL;
}
