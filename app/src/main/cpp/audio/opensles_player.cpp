//
// Created by templechen on 2019-05-13.
//

#include <android/log.h>
#include "opensles_player.h"

#define out_ch_layout_nb 2
#define out_sample_fmt AV_SAMPLE_FMT_S16


opensles_player::~opensles_player() {

}

void _playCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    auto player = (opensles_player *) context;
    auto swrContext = ((opensles_player *) context)->swrContext;
    auto queue = player->audioQueue;
    auto frame = queue->pull();
    if (frame != nullptr && swrContext != nullptr) {
        int size = av_samples_get_buffer_size(nullptr, av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO),
                                              frame->nb_samples,
                                              AV_SAMPLE_FMT_S16, 1);
        if (size > 0) {
            uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
            swr_convert(swrContext, &out_buffer, 44100, (const uint8_t **) frame->data,
                        frame->nb_samples);
//            __android_log_print(ANDROID_LOG_DEBUG, "_playCallback", "%d", player->frame->nb_samples);
            (*bq)->Enqueue(bq, out_buffer, size);
        }
    }
    av_frame_free(&frame);
}

int opensles_player::createPlayer() {
    //创建播放器
    //创建并且初始化引擎对象
//    SLObjectItf engineObject;
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //获取引擎接口
//    SLEngineItf engineItf;
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineItf);
    //通过引擎接口获取输出混音
//    SLObjectItf mixObject;
    (*engineItf)->CreateOutputMix(engineItf, &mixObject, 0, 0, 0);
    (*mixObject)->Realize(mixObject, SL_BOOLEAN_FALSE);

    //设置播放器参数
    SLDataLocator_AndroidSimpleBufferQueue
            android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLuint32 samplesPerSec = (SLuint32) 44100 * 1000;
    //pcm格式
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM,
                            2,//两声道
                            samplesPerSec,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//
                            SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource slDataSource = {&android_queue, &pcm};

    //输出管道
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObject};
    SLDataSink audioSnk = {&outputMix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    //通过引擎接口，创建并且初始化播放器对象
//    SLObjectItf playerObject;
    (*engineItf)->CreateAudioPlayer(engineItf, &playerObject, &slDataSource, &audioSnk, 1, ids,
                                    req);
    (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);

    //获取播放接口
//    SLPlayItf playItf;
    (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playItf);
    //获取缓冲接口
//    SLAndroidSimpleBufferQueueItf bufferQueueItf;
    (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &bufferQueueItf);

    //注册缓冲回调
    (*bufferQueueItf)->RegisterCallback(bufferQueueItf, _playCallback, this);
    (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    return 1;
}

void opensles_player::play() {
    (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    _playCallback(bufferQueueItf, this);
}


