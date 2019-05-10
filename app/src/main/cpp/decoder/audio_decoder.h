//
// Created by templechen on 2019-05-04.
//

#ifndef FFMPEG_PLAYER_AUDIO_DECODER_H
#define FFMPEG_PLAYER_AUDIO_DECODER_H

#include <sys/types.h>
#include "circle_av_frame_queue.h"

class audio_decoder {

public:
    audio_decoder();

    virtual ~audio_decoder();

    void decode(const char *url, circle_av_frame_queue *video_queue);

private:
    pthread_t worker_thread;

    static void *trampoline(void *p);

    const char *url;

    circle_av_frame_queue *audio_queue;
};

#endif //FFMPEG_PLAYER_AUDIO_DECODER_H
