cmake_minimum_required(VERSION 3.10)
project(demo3)

set(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_COMPILER /usr/bin/g++)
set(FFMPEG_DIR /usr/local/Cellar/ffmpeg/6.0 CACHE PATH "ffmpeg dir")
set(SDL_DIR /usr/local/Cellar/sdl2/2.26.5 CACHE PATH "sdl dir")

set(FFMPEG_DEMO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DRESOURCES_DIR="${FFMPEG_DEMO_ROOT}/resources/")

include_directories(${FFMPEG_DEMO_ROOT}/include/ ${FFMPEG_DIR}/include/ ${SDL_DIR}/include/)

link_directories(${FFMPEG_DIR}/lib/ ${SDL_DIR}/lib/)

set(FFMPEG_DEMO_SRC 
${FFMPEG_DEMO_ROOT}/src/main.cpp 
${FFMPEG_DEMO_ROOT}/src/log.cpp 
${FFMPEG_DEMO_ROOT}/src/avframequeue.cpp 
${FFMPEG_DEMO_ROOT}/src/avpacketqueue.cpp 
${FFMPEG_DEMO_ROOT}/src/demuxthread.cpp 
${FFMPEG_DEMO_ROOT}/src/audiooutput.cpp 
${FFMPEG_DEMO_ROOT}/src/decodethread.cpp 
${FFMPEG_DEMO_ROOT}/src/videooutput.cpp)

add_executable(demo3 ${FFMPEG_DEMO_SRC})

set(FFMPEG_LIBS
        ${FFMPEG_DIR}/lib/libavcodec.dylib
        ${FFMPEG_DIR}/lib/libavformat.dylib
        ${FFMPEG_DIR}/lib/libswscale.dylib
        ${FFMPEG_DIR}/lib/libavdevice.dylib
        ${FFMPEG_DIR}/lib/libavfilter.dylib
        ${FFMPEG_DIR}/lib/libavutil.dylib
        ${FFMPEG_DIR}/lib/libswresample.dylib)

set(SDL_LIBS
        ${SDL_DIR}/lib/libSDL2-2.0.0.dylib
        ${SDL_DIR}/lib/libSDL2.dylib)


target_link_libraries(demo3
                avcodec
                avfilter
                avformat
                avutil
                swresample
                swscale
                # SDL 
                SDL2
                SDL2main
                SDL2_ttf)