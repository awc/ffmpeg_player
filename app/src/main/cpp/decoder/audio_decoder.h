//
// Created by templechen on 2019-05-04.
//

#ifndef FFMPEG_PLAYER_AUDIO_DECODER_H
#define FFMPEG_PLAYER_AUDIO_DECODER_H

#include <sys/types.h>

class audio_decoder {

public:
    audio_decoder();

    virtual ~audio_decoder();

    void decode(const char *url);

private:
    pthread_t worker_thread;

    static void *trampoline(void *p);

    const char *url;
};

#endif //FFMPEG_PLAYER_AUDIO_DECODER_H
