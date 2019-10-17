//
// Created by templechen on 2019-04-28.
//

#ifndef FFMPEG_PLAYER_GL_RENDERER_H
#define FFMPEG_PLAYER_GL_RENDERER_H

#include <android/native_window.h>
#include "../egl/egl_core.h"
#include "../egl/window_surface.h"
#include "../filter/base_filter.h"

extern "C" {
#include <libavutil/frame.h>
};

class gl_renderer {

public:
    gl_renderer();

    virtual ~gl_renderer();

    void surfaceCreated(ANativeWindow *nativeWindow);

    void surfaceChanged(int width, int height);

    void surfaceDestroyed();

    void surfaceDoFrame(AVFrame *avFrame);

private:

    egl_core *eglCore;

    window_surface *windowSurface;

    base_filter *filter;

    void initFilter(AVFrame *avFrame);

    int screen_width, screen_height;
};

#endif //FFMPEG_PLAYER_GL_RENDERER_H
