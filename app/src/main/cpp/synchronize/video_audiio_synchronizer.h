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

private:

    bool syncVideo(long pts, long currentTimeMillis);

    bool syncAudio(long pts, long currentTimeMillis);

};

#endif //FFMPEG_PLAYER_VIDEO_AUDIIO_SYNCHRONIZER_H
