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
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, url, nullptr, nullptr) < 0) {
        ALOGD("can not open", url)
        return nullptr;
    }
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        ALOGD("can not find stream info", url)
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
        ALOGD("can not find video stream info")
        return nullptr;
    }
    AVCodecContext *videoCodecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(videoCodecContext, formatContext->streams[video_stream_index]->codecpar);
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);
    int size = videoCodecContext->width * videoCodecContext->height;
    auto *packet = static_cast<AVPacket *>(malloc(sizeof(AVPacket)));
    av_new_packet(packet, size);
    //audio
    int audio_stream_index = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    if (audio_stream_index == -1) {
        ALOGD("can not find audio stream info")
        return nullptr;
    }


}
