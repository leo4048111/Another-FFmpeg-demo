//
// Created by 李茹玥 on 2023/7/14.
//

#ifndef DEMO2_VIDEOOUTPUT_H
#define DEMO2_VIDEOOUTPUT_H

#ifdef __cplusplus
extern "C"{
#include <SDL2/SDL.h>
};
#endif

#include "log.h"
#include "avframequeue.h"

class VideoOutput
{
public:
    VideoOutput(AVFrameQueue* frame_queue, int video_width, int video_height);
    ~VideoOutput();
    int Init();
    int MainLoop();
    void RefreahLoopWaitEvent(SDL_Event* event);
private:
    void videoRefresh(double *remaining_time);
    AVFrameQueue* frame_queue_=NULL;
    SDL_Event event_;
    SDL_Rect rect_;
    SDL_Window* win_=NULL;
    SDL_Renderer* renderer_=NULL;
    SDL_Texture* texture_=NULL;
    int video_width_=0;
    int video_height_=0;
    uint8_t* yuv_buf_=NULL;
    int yuv_buf_size_=0;

    // 一般窗口程序都是这么搞的，Quit事件的时候把一个boolean的值进行设置，然后event loop里面检测到这个值被设置了就退出
    bool quit_ {false};

    
};




#endif //DEMO2_VIDEOOUTPUT_H
