#define SDL_MAIN_HANDLED

#include <iostream>
#include <string>
#include <filesystem>
#include "log.h"
#include "demuxthread.h"
#include "avframequeue.h"
#include "decodethread.h"
#include "audiooutput.h"
#include "videooutput.h"

#ifdef __cplusplus
extern "C"
{
#include <libavcodec/avcodec.h>
};
#endif

int main(int argc, char **argv)
{
    LogInit();
    int ret = 0;

    AVPacketQueue audio_packet_queue;
    AVPacketQueue video_packet_queue;

    AVFrameQueue audio_frame_queue;
    AVFrameQueue video_frame_queue;

    std::string url;
    if(argc == 1)
    {
        auto fname = std::filesystem::path("test.mp4");
        auto tmp = std::filesystem::path(RESOURCES_DIR) / fname;
        url = tmp.string();
    }
    else 
        url = argv[1];

    // 解复用
    DemuxThread *demux_thread = new DemuxThread(&audio_packet_queue, &video_packet_queue);
    ret = demux_thread->Init(url.c_str());
    if (ret < 0)
    {
        Serialize("demux_thread->Init(url) failed");
        return -1;
    }

    ret = demux_thread->Start();
    if (ret < 0)
    {
        Serialize("demux_thread->Start() failed");
        return -1;
    }

    // 解码线程初始化
    // 代码中尽量不要出现手动的new，考虑使用智能指针，或者直接把对象放在栈上，一定要手动new/delete的话建议上层再进行一层RAII封装
    DecodeThread *audio_decode_thread = new DecodeThread(&audio_packet_queue, &audio_frame_queue);

    ret = audio_decode_thread->Init(demux_thread->AudioCodecParameters());
    if (ret < 0)
    {
        Serialize("audio_decode_thread->Init(demux_thread->AudioCodecParameters()) failed");
        return -1;
    }

    ret = audio_decode_thread->Start();
    if (ret < 0)
    {
        Serialize("audio_decode_thread->Start() failed");
        return -1;
    }

    DecodeThread *video_decode_thread = new DecodeThread(&video_packet_queue, &video_frame_queue);

    ret = video_decode_thread->Init(demux_thread->VideoCodecParameters());
    if (ret < 0)
    {
        Serialize("video_decode_thread->Init(demux_thread->VideoCodecParameters()) failed");
        return -1;
    }

    ret = video_decode_thread->Start();
    if (ret < 0)
    {
        Serialize("video_decode_thread->Start() failed");
        return -1;
    }

    // audio输出

    AudioParams audio_params = {0};
    memset(&audio_params, 0, sizeof(AudioParams));
    audio_params.channels = demux_thread->AudioCodecParameters()->ch_layout.nb_channels;
    av_channel_layout_copy((AVChannelLayout *)&audio_params.channel_layout, &demux_thread->AudioCodecParameters()->ch_layout);
    //    audio_params.channel_layout=(AVChannelLayout)(demux_thread->AudioCodecParameters()->ch_layout);
    audio_params.fmt = (enum AVSampleFormat)demux_thread->AudioCodecParameters()->format;
    audio_params.freq = demux_thread->AudioCodecParameters()->sample_rate;
    audio_params.frame_size = demux_thread->AudioCodecParameters()->frame_size;

    AudioOutput *audio_output = new AudioOutput(audio_params, &audio_frame_queue);
    ret = audio_output->Init();
    if (ret < 0)
    {
        Serialize("audio_output->Init() failed");
        return -1;
    }

    // video输出
    VideoOutput *video_output = new VideoOutput(&video_frame_queue, demux_thread->VideoCodecParameters()->width, demux_thread->VideoCodecParameters()->height);
    ret = video_output->Init();
    if (ret < 0)
    {
        Serialize("video_output->Init() failed");
        return -1;
    }
    video_output->MainLoop();

    // // 睡眠2秒，意义不明，注释掉了
    // std::this_thread::sleep_for(std::chrono::milliseconds(50000));
    Serialize("demux_thread->Stop");
    demux_thread->Stop();
    Serialize("delete demux_thread");
    delete demux_thread;

    // Serialize("audio_decode_thread->Stop");
    // audio_decode_thread->Stop();
    // Serialize("delete audio_decode_thread");
    // delete audio_decode_thread;

    Serialize("video_decode_thread->Stop");
    video_decode_thread->Stop();
    Serialize("delete video_decode_thread");
    delete video_decode_thread;

    Serialize("main finish");

    return 0;
}
