//
// Created by templechen on 2019-05-06.
//

#ifndef FFMPEG_PLAYER_CIRCLE_AV_FRAME_QUEUE_H
#define FFMPEG_PLAYER_CIRCLE_AV_FRAME_QUEUE_H

extern "C" {
#include "libavformat/avformat.h"
};

#define CIRCLE_QUEUE_DEFAULT_SIZE 20
#define CIRCLE_QUEUE_STOP_INTERVAL 10

struct H264Frame {
    AVFrame *frame;
    H264Frame *next;
};

class circle_av_frame_queue {

public:

    circle_av_frame_queue();

    virtual ~circle_av_frame_queue();

    void push(AVFrame *avFrame);

    AVFrame *pull();

    int64_t pullAVFramePts();

private:

    pthread_mutex_t mLock;

    pthread_cond_t mCondition;

    H264Frame *pullCursor;

    H264Frame *pushCursor;

    H264Frame *head;

    H264Frame *tail;

    bool isAvailable;

};

#endif //FFMPEG_PLAYER_CIRCLE_AV_FRAME_QUEUE_H
