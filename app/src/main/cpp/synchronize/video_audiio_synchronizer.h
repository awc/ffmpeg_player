//
// Created by templechen on 2019-05-07.
//

#ifndef FFMPEG_PLAYER_VIDEO_AUDIIO_SYNCHRONIZER_H
#define FFMPEG_PLAYER_VIDEO_AUDIIO_SYNCHRONIZER_H

extern "C" {
#include "libavformat/avformat.h"
};

class video_audio_synchronizer {

public:

    video_audio_synchronizer();

    virtual ~video_audio_synchronizer();

    bool syncVideo(int64_t pts, int64_t currentTimeMillis);

    bool syncAudio(int64_t pts, int64_t currentTimeMillis);

};

#endif //FFMPEG_PLAYER_VIDEO_AUDIIO_SYNCHRONIZER_H
