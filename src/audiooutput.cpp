//
// Created by 李茹玥 on 2023/7/13.
//
#include "audiooutput.h"
#include "log.h"
#include "state.h"

#ifdef __cplusplus
extern "C"
{
#include "libavutil/time.h"
}
#endif

// FILE * dump_pcm=NULL;
// char* path="/Users/liruyue/Documents/dump.pcm";
static SDL_AudioDeviceID audio_dev;

AudioOutput::AudioOutput(const AudioParams &audio_params, AVFrameQueue *frame_queue)
{
    src_tgt_ = audio_params;
    frame_queue_ = frame_queue;
    State::GetInstance().audio_frame_queue = frame_queue_;
}

AudioOutput::~AudioOutput()
{
}

// 回调函数
//  void fill_audio_pcm(void* udata, Uint8* stream, int len){
//      AudioOutput* is=(AudioOutput*)udata;
//      int len1=0;
//      int audio_size=0;
//      // if(!dump_pcm){
//      //     dump_pcm= fopen(path,"wb");
//      // }
//      Serialize("fill pcm len:%d",len);
//      while(len>0){
//          if(is->audio_buf_index==is->audio_buf_size){//如果buf满了，判断是否需要重采样，重采样之后再写入pcm文件
//              is->audio_buf_index=0;
//              //读到帧
//              AVFrame* frame=is->frame_queue_->Pop(10);
//              if(frame){
//                  //av_channel_layout_compare两个layout相等返回0，不等返回1
//                  if(((frame->format!=is->dst_tgt_.fmt)
//                  ||(frame->sample_rate!=is->dst_tgt_.freq)
//                    || (av_channel_layout_compare(&frame->ch_layout,
//                                                  reinterpret_cast<const AVChannelLayout *>(&is->dst_tgt_.channel_layout)))) && (!is->swr_cxt_)){//frame->channel_layout!=is->dst_tgt_.channel_layout)
//                      //设置输入输出参数
//  //                    is->swr_cxt_=swr_alloc();
//                      swr_alloc_set_opts2(&is->swr_cxt_,
//                                          reinterpret_cast<const AVChannelLayout *>(&is->dst_tgt_.channel_layout),
//                               (enum AVSampleFormat) is->dst_tgt_.fmt,
//                               is->dst_tgt_.freq,
//                                          &frame->ch_layout,
//                               (enum AVSampleFormat) frame->format,
//                               frame->sample_rate,
//                               0, NULL);
//                      //初始化SwrContext
//                      if(!is->swr_cxt_|| swr_init(is->swr_cxt_)<0){
//                          Serialize("Fail to create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels\n",
//                                    frame->sample_rate,
//                                    av_get_sample_fmt_name((enum AVSampleFormat)frame->format),
//                                    frame->ch_layout.nb_channels,
//                                            is->dst_tgt_.freq,
//                                    av_get_sample_fmt_name((enum AVSampleFormat)is->dst_tgt_.fmt),
//                                            is->dst_tgt_.channels);
//                          swr_free(&is->swr_cxt_);
//                          return;
//                      }
//                  }
//                  if(is->swr_cxt_){
//                      const uint8_t **in=(const uint8_t**)frame->extended_data;
//                      uint8_t **out =&is->audio_buf1_;
//                      int out_samples=frame->nb_samples*is->dst_tgt_.freq/frame->sample_rate+256;
//                      int out_bytes= av_samples_get_buffer_size(NULL,is->dst_tgt_.channels,out_samples,(enum AVSampleFormat)is->dst_tgt_.fmt,0);
//                      if(out_bytes<0){
//                          Serialize("av_samples_get_buffer_size() failed");
//                          return;
//                      }

//                     //有重采样
//                     //分配内存
// //                    audio_size= av_samples_get_buffer_size(NULL,frame->ch_layout.nb_channels,frame->nb_samples,(enum AVSampleFormat)frame->format,1);
//                     av_fast_malloc(&is->audio_buf1_, (unsigned int*)(&is->audio_buf1_size), out_bytes);
// //                    is->audio_buf_=is->audio_buf1_;
// //                    is->audio_buf_size=audio_size;
//                     //进行重采样
//                     int len2= swr_convert(is->swr_cxt_,out,out_samples,in,frame->nb_samples);
//                     if(len2<0){
//                         Serialize("swr_convert failed");
//                         return;
//                     }
//                     is->audio_buf_=is->audio_buf1_;
//                     is->audio_buf_size= av_samples_get_buffer_size(NULL,is->dst_tgt_.channels,len2,is->dst_tgt_.fmt,1);
//                 }
//                 else{
//                     //没有重采样
//                     audio_size= av_samples_get_buffer_size(NULL,frame->ch_layout.nb_channels,frame->nb_samples,(enum AVSampleFormat)frame->format,1);
//                     av_fast_malloc(&is->audio_buf1_, (unsigned int *)(&is->audio_buf1_size), audio_size);
//                     is->audio_buf_=is->audio_buf1_;
//                     is->audio_buf_size=audio_size;
//                     memcpy(is->audio_buf_, frame->data[0],audio_size);
//                 }
//                 av_frame_free(&frame);
//             }
//             else{//没有读到帧
//                 is->audio_buf_=NULL;
//                 is->audio_buf_size=512;
//             }
//         }
//         len1=is->audio_buf_size-is->audio_buf_index;//buf中剩余的空间
//         if(len1>len){
//             len1=len;//取剩余空间与数据长度的最小值，即len1为写的数据长度
//         }
// //        if(len1<0){
// //            len1=0;
// //        }
//         if(!is->audio_buf_){
//             memset(stream,0,len1);//分配len1大小的空间
//         }
//         else{
//             memcpy(stream, is->audio_buf_+is->audio_buf_index,len1);
//             //对音乐数据混音
//             SDL_MixAudio(stream, is->audio_buf_+is->audio_buf_index,len1, SDL_MIX_MAXVOLUME/8);
//             // fwrite((uint8_t*)is->audio_buf_+is->audio_buf_index,1,len1,dump_pcm);
//             // fflush(dump_pcm);
//         }
//         len-=len1;
//         stream+=len1;
//         is->audio_buf_index+=len1;
//     }
// }
/*
void fill_audio_pcm(void* udata, Uint8* stream, int len){
    SDL_memset(stream,0,len);
    if(audio_pos_>=audio_end_){
        return;
    }
    //数据够了就读预设长度，数据不够就只读部分（不够的时候剩多少就读取多少）
    int remain_buffer_len=audio_end_-audio_pos_;
    len=(len<remain_buffer_len)?len:remain_buffer_len;
    // 拷贝数据到stream并调整音量（参数4是音量调节）
    SDL_MixAudio(stream, audio_pos_,len, SDL_MIX_MAXVOLUME/8);
    Serialize("len=%d",len);
    audio_pos_+=len;
    return;
}*/

/*
int AudioOutput::Init() {
    if(SDL_Init(SDL_INIT_AUDIO)!=0){
        Serialize("SDL_Init failed");
        return -1;
    }

    FILE* audio_fd= fopen(path,"rb");
    if(!audio_fd){
        Serialize("fail to open pcm file");
        return -1;
    }

    SDL_AudioSpec spec;
    size_t read_buffer_len=0;

    audio_buf_=(uint8_t*)malloc(PCM_BUFFER_SIZE);
    spec.freq=44100;
    spec.format=AUDIO_S16SYS;
    spec.channels=2;
    spec.silence=0;
    spec.samples=1024;
    spec.callback=NULL;
    spec.userdata=NULL;

    int ret= SDL_OpenAudio(&spec,NULL);
    if(ret!=0){
        Serialize("SDL_OpenAudio failed");
        return -1;
    }

    SDL_PauseAudio(0);//暂停播放，或播放。根据参数来决定，如果参数是非0值就暂停，如果是0值就播放。
    int data_count=0;
    while(true){
        read_buffer_len= fread(audio_buf_,1,PCM_BUFFER_SIZE,audio_fd);
        if(read_buffer_len==0){
            break;
        }
        data_count+=read_buffer_len;
        Serialize("now playing %d bytes data.",data_count);
        audio_end_=audio_buf_+read_buffer_len;
        audio_pos_=audio_buf_;
        while(audio_pos_<audio_end_){
            SDL_Delay(10);
        }
    }
    Serialize("play PCM finish");
    SDL_CloseAudio();
    if(audio_buf_){
        free(audio_buf_);
    }
    if(audio_fd){
        fclose(audio_fd);
    }
    SDL_Quit();
    return 0;
}
*/

int AudioOutput::DeInit()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    Serialize("AudioOutput::DeInit");
    return 0;
}

// 最简单的实现
void fill_audio_pcm(void *udata, Uint8 *stream, int len)
{
    AudioOutput *is = (AudioOutput *)(udata);
    State &state = State::GetInstance();
    // 这里我乱写的，你还要自己再研究一下，audio clock是怎么设置的
    static double start_time = av_gettime_relative() / 1000000.0;
    state.audio_start_time = start_time;
    state.audio_clock = av_gettime_relative() / 1000000.0;
    AVFrameQueue *frame_queue = state.audio_frame_queue;
    while (len)
    {
        int size = frame_queue->Size();
        if (!size)
        {
            memset(stream, 0, len);
            len = 0;
        }
        // else
        // {
        //     auto frame = frame_queue->Pop(10);
        //     int bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)frame->format);
        //     for (int i = 0; i < frame->nb_samples; ++i)
        //     {
        //         for (int ch = 0; ch < 2; ++ch)
        //         {
        //             memcpy_s(stream, bytesPerSample, frame->data[ch] + i * bytesPerSample, bytesPerSample);
        //             stream += bytesPerSample;
        //         }
        //     }
        //     len = 0;
        // }
        else
        {
            // 缓存里还有数据，先把缓存里的送掉
            if (state.audio_buf_index < state.audio_buf_size)
            {
                int len1 = state.audio_buf_size - state.audio_buf_index;
                len1 = (len1 < len) ? len1 : len;
                memcpy(stream, state.audio_buf_ + state.audio_buf_index, len1);
                stream += len1;
                len -= len1;
                state.audio_buf_index += len1;
            }
            else // 缓存里数据不够了，拿一个Frame出来取新数据
            {
                state.audio_buf_index = 0;
                auto frame = frame_queue->Pop(10);
                int bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)frame->format);
                // size_t size = frame->nb_samples * bytesPerSample * is->spec_.channels;
                size_t size = av_samples_get_buffer_size(NULL, frame->ch_layout.nb_channels,
                                                         frame->nb_samples,
                                                         (AVSampleFormat)frame->format, 1);

                state.audio_buf_size = size;
                if (state.audio_buf_ != nullptr)
                {
                    av_free(state.audio_buf_);
                    state.audio_buf_ = nullptr;
                }

                if (state.audio_buf_ == nullptr)
                    state.audio_buf_ = (uint8_t *)av_malloc(size);

                int tmpIdx = 0;
                for (int i = 0; i < frame->nb_samples; ++i)
                {
                    for (int ch = 0; ch < is->spec_.channels; ++ch)
                    {
                        memcpy_s((void *)&state.audio_buf_[tmpIdx], bytesPerSample, (const char *)frame->data[ch] + i * bytesPerSample, bytesPerSample);
                        tmpIdx += bytesPerSample;
                    }
                }

                av_frame_free(&frame);
            }
        }
    }
}

int AudioOutput::Init()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        Serialize("SDL_Init failed");
        return -1;
    }

    SDL_AudioSpec wanted_spec;
    wanted_spec.channels = 2; // 只支持2channel的输出
    wanted_spec.freq = src_tgt_.freq;
    wanted_spec.format = AUDIO_F32SYS;
    wanted_spec.silence = 0;
    wanted_spec.callback = fill_audio_pcm;
    wanted_spec.userdata = this;
    wanted_spec.samples = 4096; // 采样数量

    int ret = SDL_OpenAudio(&wanted_spec, NULL);
    if (ret != 0)
    {
        Serialize("SDL_OpenAudio failed");
        return -1;
    }
    spec_.channels = wanted_spec.channels;
    dst_tgt_.channels = spec_.channels; // spec.channels;
    dst_tgt_.fmt = AV_SAMPLE_FMT_S32;
    dst_tgt_.freq = spec_.freq; // spec.freq;
    av_channel_layout_default((AVChannelLayout *)(&dst_tgt_.channel_layout), 2);
    //    dst_tgt_.channel_layout = av_get_default_channel_layout(2);
    dst_tgt_.frame_size = 1024; // src_tgt_.frame_size;
    SDL_PauseAudio(0);
    Serialize("AudioOutput::Init leave");
    return 0;
}