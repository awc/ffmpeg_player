//
// Created by templechen on 2019-05-09.
//

#ifndef FFMPEG_PLAYER_AUDIO_LOOPER_H
#define FFMPEG_PLAYER_AUDIO_LOOPER_H

#include "../render/looper.h"
#include "../decoder/circle_av_frame_queue.h"
#include "opensles_player.h"

class audio_looper : public looper {

public:

    enum {
        kMsgAudioPlayerCreated,
        kMsgAudioPlayerDestroyed,
        kMsgAudioPlayerDoFrame,
        kMsgSwrContextInit
    };

    audio_looper(circle_av_frame_queue *audioQueue);

    virtual ~audio_looper();

    void handleMessage(LooperMessage *msg) override;

private:

    circle_av_frame_queue *audioQueue;

    opensles_player *openslesPlayer = nullptr;
};

#endif //FFMPEG_PLAYER_AUDIO_LOOPER_H
