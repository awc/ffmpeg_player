//
// Created by templechen on 2019-05-09.
//

#include "audio_looper.h"

audio_looper::audio_looper(circle_av_frame_queue *audioQueue) {
    this->audioQueue = audioQueue;
}

audio_looper::~audio_looper() {

}

#define USE_OBOE 0

void audio_looper::handleMessage(looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgAudioPlayerCreated: {
            if (USE_OBOE) {
                oboePlayer = new audio_player(msg->arg1, msg->arg2, 2, static_cast<circle_av_frame_queue *>(msg->obj));
            } else {
                openslesPlayer = new opensles_player(audioQueue);
                openslesPlayer->createPlayer();
            }
            break;
        }
        case kMsgAudioPlayerDestroyed: {
            if (USE_OBOE) {
                if (oboePlayer != nullptr) {
                    delete oboePlayer;
                    oboePlayer = nullptr;
                }
            } else {
                if (openslesPlayer != nullptr) {
                    delete openslesPlayer;
                    openslesPlayer = nullptr;
                }
            }
            break;
        }
        case kMsgAudioPlayerDoFrame: {
            if (USE_OBOE) {

            } else {
                if (openslesPlayer != nullptr) {
                    openslesPlayer->play();
                }
            }
            break;
        }
        case kMsgSwrContextInit: {
            if (USE_OBOE) {

            } else {
                if (openslesPlayer != nullptr) {
                    openslesPlayer->swrContext = static_cast<SwrContext *>(msg->obj);
                }
            }
            break;
        }
        default:
            break;
    }
}
