//
// Created by templechen on 2019-06-03.
//

#include <pthread.h>
#include "av_demuxer.h"
#include "../common/native_log.h"

extern "C" {
#include <libavformat/avformat.h>
}

av_demuxer::av_demuxer() {

}

av_demuxer::~av_demuxer() {

}

void av_demuxer::decode(const char *url, circle_av_packet_queue *video_queue, circle_av_packet_queue *audio_queue) {
    this->url = url;
    this->video_queue = video_queue;
    this->audio_queue = audio_queue;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker_thread, &attr, trampoline, this);
}

void *av_demuxer::trampoline(void *p) {
    avformat_network_init();
    const char *url = ((av_demuxer *) p)->url;
    circle_av_packet_queue *video_queue = ((av_demuxer *) p)->video_queue;
    circle_av_packet_queue *audio_queue = ((av_demuxer *) p)->audio_queue;
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, url, nullptr, nullptr) < 0) {
        ALOGD("can not open %s, %s:%s", url, __FILE__, __func__);
        return nullptr;
    }
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        ALOGD("can not find stream info %s", url);
        return nullptr;
    }
    //video
    int video_stream_index = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        ALOGD("can not find video stream info");
        return nullptr;
    }
    //audio
    int audio_stream_index = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    if (audio_stream_index == -1) {
        ALOGD("can not find audio stream info");
        return nullptr;
    }

    while (true) {
        auto *packet = av_packet_alloc();
        if (av_read_frame(formatContext, packet) < 0) {
            ALOGD("read frame end");
            break;
        }
        if (packet->stream_index == video_stream_index) {
            video_queue->push(packet);
        } else if (packet->stream_index == audio_stream_index) {
            audio_queue->push(packet);
        }
    }
    avformat_close_input(&formatContext);

    __android_log_print(ANDROID_LOG_DEBUG, "audio & video", " avPacket push over");
    
    return nullptr;
}
