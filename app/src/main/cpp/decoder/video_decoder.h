//
// Created by templechen on 2019-04-30.
//

#ifndef FFMPEG_PLAYER_FFMPEG_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_DECODER_H

#include <string>
#include <jni.h>
#include "../render/gl_looper.h"
#include "circle_av_frame_queue.h"

class video_decoder {

public:
    video_decoder();

    virtual ~video_decoder();

    void decode(const char *url, circle_av_frame_queue *video_queue, jobject javaPlayerRef);

    JavaVM *vm = nullptr;

private:

    pthread_t workder_thread;

    static void *trampoline(void *p);

    const char *url;

    circle_av_frame_queue *video_queue;

    JNIEnv *env = nullptr;

    jobject javaPlayer;
};

#endif //FFMPEG_PLAYER_FFMPEG_DECODER_H
