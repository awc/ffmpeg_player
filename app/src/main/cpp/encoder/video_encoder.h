//
// Created by templechen on 2019-05-16.
//

#ifndef FFMPEG_PLAYER_VIDEO_ENCODER_H
#define FFMPEG_PLAYER_VIDEO_ENCODER_H

extern "C" {
#include <libavutil/frame.h>
}

class video_encoder {

public:

    video_encoder(const char *dest);

    virtual ~video_encoder();

    void sendFrame(AVFrame *frame);
};

#endif //FFMPEG_PLAYER_VIDEO_ENCODER_H
