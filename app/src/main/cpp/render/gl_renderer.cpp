//
// Created by templechen on 2019-04-28.
//

#include "gl_renderer.h"
#include "../filter/mediacodec_filter.h"
#include <GLES3/gl3.h>

gl_renderer::gl_renderer() {
    eglCore = nullptr;
    windowSurface = nullptr;
}

gl_renderer::~gl_renderer() {
    if (filter != nullptr) {
        delete filter;
        filter = nullptr;
    }
    if (windowSurface != nullptr) {
        windowSurface->release();
        delete windowSurface;
        windowSurface = nullptr;
    }
    if (eglCore != nullptr) {
        eglCore->release();
        delete eglCore;
        eglCore = nullptr;
    }
}

void gl_renderer::surfaceCreated(ANativeWindow *nativeWindow) {
    eglCore = new egl_core(nullptr, FLAG_TRY_GLES3);
    windowSurface = new window_surface(nativeWindow, eglCore, false);
    windowSurface->makeCurrent();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    filter = new base_filter();
    filter = new mediacodec_filter();
    filter->init_program();

}

void gl_renderer::surfaceChanged(int width, int height) {
    windowSurface->makeCurrent();
    glViewport(0, 0, width, height);
    windowSurface->swapBuffer();

    filter->screen_width = width;
    filter->screen_height = height;
}

void gl_renderer::surfaceDestroyed() {
//    if (filter != nullptr) {
//        delete filter;
//        filter = nullptr;
//    }
//    if (windowSurface != nullptr) {
//        windowSurface->release();
//        delete windowSurface;
//        windowSurface = nullptr;
//    }
//    if (eglCore != nullptr) {
//        eglCore->release();
//        delete eglCore;
//        eglCore = nullptr;
//    }
}

void gl_renderer::surfaceDoFrame(AVFrame *avFrame) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    filter->drawFrame(avFrame);

    windowSurface->swapBuffer();
}
