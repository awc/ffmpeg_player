//
// Created by templechen on 2019-05-04.
//

#include <pthread.h>
#include "audio_decoder.h"
#include "../common/native_log.h"

extern "C" {
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"

}
#define MAX_AUDIO_FRAME_SIZE 192000

audio_decoder::audio_decoder() {

}

audio_decoder::~audio_decoder() {

}

void audio_decoder::decode(const char *url) {
    this->url = url;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker_thread, &attr, trampoline, this);
}

void *audio_decoder::trampoline(void *p) {
    const char *url = ((audio_decoder *) p)->url;
    //封装格式上下文
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, url, nullptr, nullptr) < 0) {
        ALOGD("can not open ", url)
        return nullptr;
    }
    //获取信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        ALOGD("can not find stream info", url)
        return nullptr;
    }
    //获取音频流的索引位置
    int audio_stream_index = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    if (audio_stream_index == -1) {
        ALOGD("can not find video stream info")
        return nullptr;
    }
    AVCodecContext *codecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codecContext, formatContext->streams[audio_stream_index]->codecpar);
    AVCodec *videoCodec = avcodec_find_decoder(codecContext->codec_id);
    //打开解码器
    if (avcodec_open2(codecContext, videoCodec, nullptr) < 0) {
        ALOGD("can not open video decoder")
        return nullptr;
    }

    AVPacket *packet = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();

    //Out Audio Param
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    //AAC:1024  MP3:1152
    int out_nb_samples = codecContext->frame_size;
    AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = 44100;
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

    //FIX:Some Codec's Context Information is missing
    int64_t in_channel_layout=av_get_default_channel_layout(codecContext->channels);
    SwrContext *swrContext = swr_alloc_set_opts(nullptr, out_channel_layout, out_sample_fmt, out_sample_rate,
                                                in_channel_layout, codecContext->sample_fmt,
                                                codecContext->sample_rate, 0,
                                                nullptr);
    swr_init(swrContext);

    int ret = 0;
    while (true) {
        if (av_read_frame(formatContext, packet) < 0) {
            ALOGD("read frame end")
            break;
        }
        if (packet->stream_index == audio_stream_index) {
            avcodec_send_packet(codecContext, packet);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                ALOGD("avcodec send packet ret = %d", ret);
                av_packet_unref(packet);
                continue;
            }
            ret = avcodec_receive_frame(codecContext, pFrame);
            if (ret < 0 && ret != AVERROR_EOF) {
                ALOGD("avcodec receive frame ret = %d", ret);
                av_packet_unref(packet);
                av_frame_unref(pFrame);
                continue;
            }
            swr_convert(swrContext, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);
//            fwrite(out_buffer, 1, out_buffer_size, pFile);
            __android_log_print(ANDROID_LOG_DEBUG, "audio", " %lld, %d", pFrame->pts, packet->size);
        }
    }
    av_packet_free(&packet);
    av_frame_free(&pFrame);
    av_free(out_buffer);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    return nullptr;
}
