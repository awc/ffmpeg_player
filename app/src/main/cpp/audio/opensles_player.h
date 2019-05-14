//
// Created by templechen on 2019-05-13.
//

#ifndef FFMPEG_PLAYER_OPENSLES_PLAYER_H
#define FFMPEG_PLAYER_OPENSLES_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../decoder/circle_av_frame_queue.h"

extern "C" {
#include "libavformat/avformat.h"
#include <libswresample/swresample.h>
};

class opensles_player {

public:

    opensles_player(circle_av_frame_queue *queue) : audioQueue(queue) {}

    virtual ~opensles_player();

    int createPlayer();

    void play();

    circle_av_frame_queue *audioQueue = nullptr;

    SwrContext *swrContext = nullptr;

private:

    //Open SL ES
    SLObjectItf engineObject;       //引擎对象
    SLEngineItf engineItf;          //引擎接口
    SLObjectItf mixObject;          //输出混音对象
    SLObjectItf playerObject;       //播放器对象
    SLPlayItf playItf;              //播放器接口
    SLAndroidSimpleBufferQueueItf bufferQueueItf;   //缓冲接口

};

#endif //FFMPEG_PLAYER_OPENSLES_PLAYER_H
