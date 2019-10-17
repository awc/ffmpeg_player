//
// Created by templechen on 2019-04-28.
//

#include "gl_renderer.h"
#include "../filter/mediacodec_nv12_filter.h"
#include "../common/native_log.h"
#include "../filter/mediacodec_nv21_filter.h"
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

}

void gl_renderer::surfaceChanged(int width, int height) {
    windowSurface->makeCurrent();
    glViewport(0, 0, width, height);
    windowSurface->swapBuffer();

    screen_width = width;
    screen_height = height;
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

    initFilter(avFrame);
    filter->drawFrame(avFrame);

    windowSurface->swapBuffer();
}

void gl_renderer::initFilter(AVFrame *avFrame) {
    if (filter != nullptr) {
        return;
    }
    switch (avFrame->format) {
        case AV_PIX_FMT_YUV420P: {
            filter = new base_filter();
            break;
        }
        case AV_PIX_FMT_NV12: {
            filter = new mediacodec_nv12_filter();
            break;
        }
        case AV_PIX_FMT_NV21: {
            filter = new mediacodec_nv21_filter();
            break;
        }
        default: {
            ALOGE("not support this video type!");
            break;
        }
    }
    if (filter != nullptr) {
        filter->screen_width = screen_width;
        filter->screen_height = screen_height;
        filter->init_program();
    }
}
