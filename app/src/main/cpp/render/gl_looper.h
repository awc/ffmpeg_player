//
// Created by templechen on 2019-04-28.
//

#ifndef FFMPEG_PLAYER_GL_LOOPER_H
#define FFMPEG_PLAYER_GL_LOOPER_H


#include "looper.h"
#include "gl_renderer.h"

class gl_looper : public looper {

public:

    enum {
        kMsgSurfaceCreated,
        kMsgSurfaceChanged,
        kMsgSurfaceDestroyed,
        kMsgSurfaceDoFrame
    };

    gl_looper();

    virtual ~gl_looper();

    virtual void handleMessage(LooperMessage *msg) override;

private:
    gl_renderer *renderer;
};

#endif //FFMPEG_PLAYER_GL_LOOPER_H
