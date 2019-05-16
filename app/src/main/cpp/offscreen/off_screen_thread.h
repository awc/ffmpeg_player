//
// Created by templechen on 2019-05-16.
//

#ifndef FFMPEG_PLAYER_OFF_SCREEN_THREAD_H
#define FFMPEG_PLAYER_OFF_SCREEN_THREAD_H

#include <string>
#include <jni.h>
#include "../egl/egl_core.h"
#include "../egl/offscreen_surface.h"

class off_screen_thread {

public:
    off_screen_thread(const char *path, JavaVM *vm);

    virtual ~off_screen_thread();

private:

    const char *path;

    JavaVM *vm;

    pthread_t worker_thread;

    static void *trampoline(void *p);

};

#endif //FFMPEG_PLAYER_OFF_SCREEN_THREAD_H
