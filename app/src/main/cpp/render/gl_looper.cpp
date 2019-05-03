//
// Created by templechen on 2019-04-28.
//

#include "gl_looper.h"
extern "C" {
#include <libavutil/frame.h>
}

gl_looper::gl_looper() {
    renderer = new gl_renderer();
}

gl_looper::~gl_looper() {
    delete renderer;
    renderer = nullptr;
}

void gl_looper::handleMessage(looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgSurfaceCreated: {
            if (renderer != nullptr) {
                renderer->surfaceCreated(static_cast<ANativeWindow *>(msg->obj));
            }
            break;
        }
        case kMsgSurfaceChanged: {
            if (renderer != nullptr) {
                renderer->surfaceChanged(msg->arg1, msg->arg2);
            }
            break;
        }
        case kMsgSurfaceDestroyed: {
            if (renderer != nullptr) {
                renderer->surfaceDestroyed();
            }
            break;
        }
        case kMsgSurfaceDoFrame: {
            if (renderer != nullptr) {
                renderer->surfaceDoFrame(static_cast<AVFrame *>(msg->obj));
            }
            break;
        }
        default:
            break;
    }
}

