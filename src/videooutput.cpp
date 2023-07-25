//
// Created by 李茹玥 on 2023/7/14.
//
// 如何把每一帧yuv数据保存在文件中？
//
// frame_yuv->data[0] 是个一维数组（区别于frame_yuv->data），
// 一维数组如何装下二维矩形数据呢（上面描述的矩形），视频解码后把数据一行一行地按着顺序储存frame_yuv->data[0]里面，所以我们也要一行一行读取。
//
// 第i需要偏移i * frame_yuv->linesize[0]
// 起始地址就是 frame_yuv->data[0] + i * frame_yuv->linesize[0]，
// 然后再取出width长度的数据就是第i行y分量，y分量高度为height，所以i的值从0到height
// u、v分量高度为frame_yuv->height / 2，在frame_yuv->data[1、2] 中宽度为frame_yuv->linesize[1、2] ，实际宽度为frame_yuv->height / 2。
#include <iostream>
#include <thread>

#ifdef __cplusplus
extern "C"
{
#include "libavutil/time.h"
}
#endif

#include "log.h"
#include "videooutput.h"
#include "state.h"
#define REFRESH_RATE 0.1

VideoOutput::VideoOutput(AVFrameQueue *frame_queue, int video_width, int video_height)
{
    frame_queue_ = frame_queue;
    video_width_ = video_width;
    video_height_ = video_height;
}

VideoOutput::~VideoOutput()
{
}

int VideoOutput::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    { // 视频子系统; 自动初始化事件子系统.int 成功时返回0，失败时返回负数错误码; 调用SDL_GetError()可以获得本次异常信息。
        Serialize("SDL_Init failed");
        return -1;
    }
    // SDL_WINDOW_OPENGL 与OpenGL环境一起使用的窗口
    // SDL_WINDOW_RESIZABLE 可伸缩
    win_ = SDL_CreateWindow("player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, video_width_, video_height_, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win_)
    {
        Serialize("SDL_CreateWindow failed");
        return -1;
    }

    renderer_ = SDL_CreateRenderer(win_, -1, 0);
    if (!renderer_)
    {
        Serialize("SDL_CreateRenderer failed");
        return -1;
    }

    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_width_, video_height_);
    if (!texture_)
    {
        Serialize("SDL_CreateTexture failed");
        return -1;
    }

    // YUV420SP数据大小的计算公式是: size = width * height * 1.5;
    // YUV420P数据大小的计算公式是:size = stride * height
    // 程序处理需要YUV420SP数据时，当前数据是YUV420P，那么就需要对数据进行处理。
    yuv_buf_size_ = video_width_ * video_height_ * 1.5;
    yuv_buf_ = (uint8_t *)malloc(sizeof(uint8_t) * yuv_buf_size_);
    return 0;
}

int VideoOutput::MainLoop()
{
    SDL_Event event;
    while (!quit_)
    {
        RefreahLoopWaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                Serialize("ESC key down");
                return 0;
            }
            break;
        case SDL_QUIT:
            Serialize("SDL_QUIT");
            quit_ = true;
            break;
        default:
            break;
        }
    }

    return 0;
}

void VideoOutput::RefreahLoopWaitEvent(SDL_Event *event)
{
    double remaining_time = REFRESH_RATE;
    SDL_PumpEvents(); // 更新键盘状态
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT))
    {
        // SDL_PeepEvents读取事件，调用该函数之前，必须调用SDL_PumpEvents搜索键盘等事件
        // SDL_GETEVENT读取队列中事件，并将该事件从队列中移除
        if (remaining_time > 0)
        {
            // 这里这里注意强转的对象是什么
            // std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)remaining_time*1000));
            av_usleep((int64_t)(remaining_time * 1000000.0));
        }
        remaining_time = REFRESH_RATE;
        videoRefresh(&remaining_time);
        SDL_PumpEvents();
    }
}

void VideoOutput::videoRefresh(double *remaining_time)
{
    AVFrame *frame = NULL;
    // 计算同步（简易版）
    auto& state = State::GetInstance();
    double currentTime = state.audio_clock - state.audio_start_time; // 这里currentTime是主时钟，video同步到这个时钟上,统一先换成单位秒进行计算
    auto videoStreamTimebase = State::GetInstance().video_stream_tb;
    double pts;
    double diff;
    const double syncThreshold = 0.01;
    static int64_t lastPts = 0;
    double delay;

retry:
    if (!frame_queue_->Size())
        return;

    frame = frame_queue_->Pop(1);
    if (frame)
    {
        pts = frame->pts * (double)videoStreamTimebase.num / videoStreamTimebase.den;
        diff = pts - currentTime;
        delay = (frame->pts - lastPts) * (double)videoStreamTimebase.num / videoStreamTimebase.den;
        lastPts = frame->pts;
        // too late, drop frame and rebase timestamp
        if (diff <= -syncThreshold)
        {
            if (diff > -delay)
            {
                delay += diff;
            }
            else if(diff > -5)
            {
                // diff在可控范围内，丢帧重试
                goto retry;
            } else {
                // 放弃同步
                delay = 0;
            }
        }

        // too early, wait some proper time
        if (diff >= syncThreshold)
        {
            delay = std::min(delay * 2, delay + diff);
        }

        int64_t beforeRenderTime = av_gettime_relative();
        rect_.x = 0;
        rect_.y = 0;
        rect_.w = video_width_;
        rect_.h = video_height_;
        // “Y”表示明亮度（Luminance或Luma），也就是灰阶值；而“U”和“V”表示的则是色度
        // frame->linesize是每个图片行的字节大小
        // Y：frame->data[0]
        // U：frame->data[1]
        // V：frame->data[2]
        SDL_UpdateYUVTexture(texture_, &rect_,
                             frame->data[0], frame->linesize[0],
                             frame->data[1], frame->linesize[1],
                             frame->data[2], frame->linesize[2]);
        // 清空、拷贝、展示渲染器
        SDL_RenderClear(renderer_); // This function clears the entire rendering target
        SDL_RenderCopy(renderer_, texture_, NULL, &rect_);
        SDL_RenderPresent(renderer_);
        int64_t renderTime = av_gettime_relative() - beforeRenderTime;
        delay -= renderTime / 1000000.0;
        // 删除一个frame
        av_frame_free(&frame);
        *remaining_time = delay;
    }
}
