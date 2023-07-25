//
// Created by 李茹玥 on 2023/7/12.
//
#include "decodethread.h"
#include "log.h"

DecodeThread::DecodeThread(AVPacketQueue *packet_queue, AVFrameQueue *frame_queue)
{
    packet_queue_ = packet_queue;
    frame_queue_ = frame_queue;
}

DecodeThread::~DecodeThread()
{
    Stop();
    if (codec_ctx_)
    {
        avcodec_close(codec_ctx_);
    }
}

int DecodeThread::Init(AVCodecParameters *par)
{
    if (!par)
    {
        Serialize("Init par is null");
        return -1;
    }
    codec_ctx_ = avcodec_alloc_context3(NULL);
    int ret = avcodec_parameters_to_context(codec_ctx_, par);
    if (ret < 0)
    {
        av_strerror(ret, err, sizeof(err));
        Serialize("avcodec_parameters_to_context(codec_ctx_,par) failed, ret:%d, err:%s", ret, err);
        return -1;
    }

    const AVCodec *codec = avcodec_find_decoder(codec_ctx_->codec_id);
    if (!codec)
    {
        Serialize("avcodec_find_decoder(codec_ctx_->codec_id) failed");
        return -1;
    }

    ret = avcodec_open2(codec_ctx_, codec, NULL);
    if (ret < 0)
    {
        av_strerror(ret, err, sizeof(err));
        Serialize("avcodec_open2(codec_ctx_,codec,NULL) failed");
        return -1;
    }
    Serialize("Init finish");
    return 0;
}

int DecodeThread::Start()
{
    thread_ = std::make_unique<std::thread>(&DecodeThread::Run, this);
    if (!thread_)
    {
        Serialize("new std::thread(&DecodeThread::Run,this) failed");
        return -1;
    }
    thread_->detach();
    return 0;
}

int DecodeThread::Stop()
{
    abort_ = true;
    return 0;
}

void DecodeThread::Run()
{
    Serialize("Run");

    AVFrame *frame = av_frame_alloc();

    while (!abort_)
    {
        // 意义不明，暂时注释掉
        // if (frame_queue_->Size() > 10)
        // {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //     continue;
        // }
        AVPacket *pkt = packet_queue_->Pop(10);
        if (!pkt)
        {
            Serialize("not get packet");
            continue;
        }

        int ret = avcodec_send_packet(codec_ctx_, pkt);
        while (true)
        {
            ret = avcodec_receive_frame(codec_ctx_, frame);
            if (ret == 0)
            {
                frame_queue_->Push(frame);
                Serialize("%d frame queue size %d", codec_ctx_->codec_id, frame_queue_->Size());
            }
            else // receive_frame的返回值一般只管是不是0，不是0直接再送packet就行
                break;
            // else if(ret==AVERROR(EAGAIN)){
            //     //AVERROR（EAGAIN）返回值意味着需要新的输入数据才能返回新的输出。
            //     break;
            // }
            // else{
            //     abort_=1;
            //     av_strerror(ret,err,sizeof(err));
            //     Serialize("avcodec_receive_frame(codec_ctx_,frame) failed, ret:%d, err:%s",ret,err);
            //     break;
            // }
            av_frame_unref(frame);
        }
        av_packet_free(&pkt);
    }

    av_frame_free(&frame);
    Serialize("DecodeThread run finish");
}