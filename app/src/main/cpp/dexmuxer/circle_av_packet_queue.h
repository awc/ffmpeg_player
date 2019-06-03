//
// Created by templechen on 2019-06-03.
//

#ifndef FFMPEG_PLAYER_CIRCLE_AV_PACKET_QUEUE_H
#define FFMPEG_PLAYER_CIRCLE_AV_PACKET_QUEUE_H

extern "C" {
#include <libavcodec/avcodec.h>
};

#define DEFAULT_PACKET_SIZE 100

struct H264Packet {
    AVPacket *packet;
    H264Packet *next;
};

class circle_av_packet_queue {

public:

    circle_av_packet_queue();

    virtual ~circle_av_packet_queue();

    void push(AVPacket *packet);

    AVPacket *pull();

private:

    pthread_mutex_t mLock;
    pthread_cond_t mCondition;

    H264Packet *tail;
    H264Packet *head;
    H264Packet *pullCursor;
    H264Packet *pushCursor;

    int currSize;

};

#endif //FFMPEG_PLAYER_CIRCLE_AV_PACKET_QUEUE_H
