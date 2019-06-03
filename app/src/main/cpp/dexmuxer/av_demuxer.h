//
// Created by templechen on 2019-06-03.
//

#ifndef FFMPEG_PLAYER_VIDEO_DEMUXER_H
#define FFMPEG_PLAYER_VIDEO_DEMUXER_H

#include <jni.h>
#include "circle_av_packet_queue.h"

class av_demuxer {

public:

    av_demuxer();

    virtual ~av_demuxer();

    void decode(const char *url, circle_av_packet_queue *video_queue, circle_av_packet_queue *audio_queue);

private:

    circle_av_packet_queue *video_queue;

    circle_av_packet_queue *audio_queue;

    pthread_t worker_thread;

    static void *trampoline(void *p);

    const char *url;

};

#endif //FFMPEG_PLAYER_VIDEO_DEMUXER_H
