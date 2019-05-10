//
// Created by templechen on 2019-05-08.
//

#ifndef FFMPEG_PLAYER_AUDIO_PLAYER_H
#define FFMPEG_PLAYER_AUDIO_PLAYER_H

#include "../decoder/circle_av_frame_queue.h"

extern "C" {
#include "libavformat/avformat.h"
};

class audio_player {

public:

    audio_player(int32_t sampleRate, int32_t framesPerBurst, int channelCount, circle_av_frame_queue *audioQueue);

    virtual ~audio_player();

    void addFrame(AVFrame *frame);

};

#endif //FFMPEG_PLAYER_AUDIO_PLAYER_H
