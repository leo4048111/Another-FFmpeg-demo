//
// Created by 李茹玥 on 2023/7/12.
//
#include "log.h"

#include <mutex>
#include <sstream>
#include <cstdarg>
#include <cstring>

static std::mutex mtx;

FILE* f=NULL;

void LogInit(){
    // f=fopen("log.txt","w+");
}


void Serialize(const char* fmt,...){
    char buf[1024];
    auto t=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
//    ss<<std::put_time(std::localtime(&t),"%Y-%m-%d %H.%M.%S")<<"--";
    std::string fmt_str;
    fmt_str+=fmt;
    fmt_str+="\n";

    //互斥
    mtx.lock();
    va_list arglist;
    va_start(arglist,fmt);
    vsnprintf(buf,sizeof(buf),fmt_str.c_str(),arglist);
    printf("%s",buf);

    //写日志
    // if(f){
    //     fwrite(buf,1, strlen(buf),f);
    //     fflush(f);
    // }

    va_end(arglist);
    mtx.unlock();
}