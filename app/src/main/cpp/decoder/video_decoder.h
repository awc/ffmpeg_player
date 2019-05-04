//
// Created by templechen on 2019-04-30.
//

#ifndef FFMPEG_PLAYER_FFMPEG_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_DECODER_H

#include <string>
#include "../render/gl_looper.h"

class video_decoder {

public:
    video_decoder();

    virtual ~video_decoder();

    void decode(const char *url, gl_looper *looper);

private:

    pthread_t workder_thread;

    static void *trampoline(void *p);

    const char *url;

    gl_looper *looper;
};

#endif //FFMPEG_PLAYER_FFMPEG_DECODER_H
