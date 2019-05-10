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
            audioPlayer = new audio_player(msg->arg1, msg->arg2, 2, audioQueue);
            break;
        }
        case kMsgAudioPlayerDestroyed: {
            if (audioPlayer != nullptr) {
                delete audioPlayer;
                audioPlayer = nullptr;
            }
            break;
        }
        case kMsgAudioPlayerDoFrame: {
            if (audioPlayer != nullptr) {
                audioPlayer->addFrame(reinterpret_cast<AVFrame *>(msg->arg1));
            }
            break;
        }
        default:
            break;
    }
}
