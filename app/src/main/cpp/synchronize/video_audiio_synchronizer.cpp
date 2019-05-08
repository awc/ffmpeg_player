//
// Created by templechen on 2019-05-07.
//

#include "video_audiio_synchronizer.h"

video_audio_synchronizer::video_audio_synchronizer() {

}

video_audio_synchronizer::~video_audio_synchronizer() {

}

bool video_audio_synchronizer::syncVideo(int64_t pts, int64_t currentTimeMillis) {
    if (pts <= currentTimeMillis) {
        return true;
    }
    return false;
}

bool video_audio_synchronizer::syncAudio(int64_t pts, int64_t currentTimeMillis) {
    return false;
}

