//
// Created by templechen on 2019-05-09.
//

#ifndef FFMPEG_PLAYER_AUDIO_LOOPER_H
#define FFMPEG_PLAYER_AUDIO_LOOPER_H

#include "../render/looper.h"
#include "audio_player.h"
#include "../decoder/circle_av_frame_queue.h"

class audio_looper : public looper {

public:

    enum {
        kMsgAudioPlayerCreated,
        kMsgAudioPlayerDestroyed,
        kMsgAudioPlayerDoFrame
    };

    audio_looper(circle_av_frame_queue *audioQueue);

    virtual ~audio_looper();

    void handleMessage(LooperMessage *msg) override;

private:

    audio_player *audioPlayer = nullptr;

    circle_av_frame_queue *audioQueue;
};

#endif //FFMPEG_PLAYER_AUDIO_LOOPER_H
