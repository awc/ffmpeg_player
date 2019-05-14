//
// Created by templechen on 2019-05-04.
//

#ifndef FFMPEG_PLAYER_AUDIO_DECODER_H
#define FFMPEG_PLAYER_AUDIO_DECODER_H

#include <sys/types.h>
#include "circle_av_frame_queue.h"
#include "../audio/audio_looper.h"

class audio_decoder {

public:
    audio_decoder();

    virtual ~audio_decoder();

    void decode(const char *url, circle_av_frame_queue *video_queue, audio_looper *audioLooper);

private:
    pthread_t worker_thread;

    static void *trampoline(void *p);

    const char *url;

    circle_av_frame_queue *audio_queue;

    audio_looper *audioLooper;
};

#endif //FFMPEG_PLAYER_AUDIO_DECODER_H
