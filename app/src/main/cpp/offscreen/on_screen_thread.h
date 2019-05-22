//
// Created by templechen on 2019-05-22.
//

#ifndef FFMPEG_PLAYER_ON_SCREEN_THREAD_H
#define FFMPEG_PLAYER_ON_SCREEN_THREAD_H

#include <string>
#include <jni.h>
#include "../egl/egl_core.h"
#include "../egl/offscreen_surface.h"

class on_screen_thread {

public:
    on_screen_thread(const char *sourcePath, const char *destPath, JavaVM *vm, ANativeWindow *nativeWindow);

    virtual ~on_screen_thread();

private:

    const char *path;
    const char *destPath;

    JavaVM *vm;

    pthread_t worker_thread;

    static void *trampoline(void *p);

    ANativeWindow *nativeWindow;
};

#endif //FFMPEG_PLAYER_ON_SCREEN_THREAD_H
