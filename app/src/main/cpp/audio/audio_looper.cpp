//
// Created by templechen on 2019-05-09.
//

#include "audio_looper.h"

audio_looper::audio_looper(circle_av_frame_queue *audioQueue) {
    this->audioQueue = audioQueue;
}

audio_looper::~audio_looper() {

}

void audio_looper::handleMessage(looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgAudioPlayerCreated: {
            openslesPlayer = new opensles_player(audioQueue);
            openslesPlayer->createPlayer();
            break;
        }
        case kMsgAudioPlayerDestroyed: {
            if (openslesPlayer != nullptr) {
                delete openslesPlayer;
                openslesPlayer = nullptr;
            }
            break;
        }
        case kMsgAudioPlayerDoFrame: {
            if (openslesPlayer != nullptr) {
                openslesPlayer->play();
            }
            break;
        }
        case kMsgSwrContextInit: {
            if (openslesPlayer != nullptr) {
                openslesPlayer->swrContext = static_cast<SwrContext *>(msg->obj);
            }
            break;
        }
        default:
            break;
    }
}
