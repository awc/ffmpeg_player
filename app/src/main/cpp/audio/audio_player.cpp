//
// Created by templechen on 2019-05-08.
//

#include "audio_player.h"
#include "../common/native_log.h"
#include "../decoder/circle_av_frame_queue.h"

#include <oboe/Oboe.h>

class MyCallback : public oboe::AudioStreamCallback {
public:
    MyCallback(circle_av_frame_queue *audioQueue) : audioQueue(audioQueue) {}

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
//        if (audioQueue != nullptr) {
//            AVFrame *frame = audioQueue->pull();
//            if (frame != nullptr) {
//                __android_log_print(ANDROID_LOG_DEBUG, "audio", "onAudioReady %d", numFrames);
//                audioData = frame->data[0];
//            }
//            return oboe::DataCallbackResult::Continue;
//        } else {
            // We requested AudioFormat::Float so we assume we got it. For production code always check what format
            // the stream has and cast to the appropriate type.
            auto *outputData = static_cast<float *>(audioData);
            // Generate random numbers centered around zero.
            const float amplitude = 0.2f;
            for (int i = 0; i < numFrames; ++i) {
                outputData[i] = ((float) drand48() - 0.5f) * 2 * amplitude;
            }
            return oboe::DataCallbackResult::Continue;
//        }
    }

private:

    circle_av_frame_queue *audioQueue = nullptr;
};

audio_player::audio_player(int32_t sampleRate, int32_t framesPerBurst, int channelCount,
                           circle_av_frame_queue *audioQueue) {
    oboe::DefaultStreamValues::SampleRate = sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = framesPerBurst;
    oboe::DefaultStreamValues::ChannelCount = channelCount;

    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setChannelCount(oboe::ChannelCount::Mono);

    MyCallback myCallback(audioQueue);
    builder.setCallback(&myCallback);
    builder.setFramesPerCallback(1024);

    oboe::AudioStream *stream;
    oboe::Result result = builder.openStream(&stream);
    if (result != oboe::Result::OK) {
        ALOGE("Failed to create stream. Error: %s", oboe::convertToText(result))
    }
    oboe::AudioFormat format = stream->getFormat();
    ALOGD("AudioStream format is %s", oboe::convertToText(format));

    stream->requestStart();

}

audio_player::~audio_player() {

}

void audio_player::addFrame(AVFrame *frame) {

}
