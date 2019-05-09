//
// Created by templechen on 2019-05-08.
//

#ifndef FFMPEG_PLAYER_AUDIO_PLAYER_H
#define FFMPEG_PLAYER_AUDIO_PLAYER_H

extern "C" {
#include "libavformat/avformat.h"
};

class audio_player {

    audio_player(int32_t sampleRate, int32_t framesPerBurst, int channelCount);

    virtual ~audio_player();

    void addFrame(AVFrame *frame);
};

#endif //FFMPEG_PLAYER_AUDIO_PLAYER_H
