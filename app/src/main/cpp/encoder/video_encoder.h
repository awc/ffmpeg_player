//
// Created by templechen on 2019-05-16.
//

#ifndef FFMPEG_PLAYER_VIDEO_ENCODER_H
#define FFMPEG_PLAYER_VIDEO_ENCODER_H


extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class video_encoder {

public:

    video_encoder(const char *dest, int width, int height, AVRational frameRate);

    virtual ~video_encoder();

    void encode_frame(int64_t pts);

private:

    AVCodecContext *codecContext;
    AVCodec *codec;
    SwsContext *swsContext = nullptr;
    FILE *f = nullptr;
};

#endif //FFMPEG_PLAYER_VIDEO_ENCODER_H
