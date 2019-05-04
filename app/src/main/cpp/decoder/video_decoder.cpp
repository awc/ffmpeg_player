//
// Created by templechen on 2019-04-30.
//

#include <pthread.h>
#include "video_decoder.h"
#include "../common/native_log.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

video_decoder::video_decoder() {

}

video_decoder::~video_decoder() {

}

void video_decoder::decode(const char *url, gl_looper *looper) {
    this->url = url;
    this->looper = looper;
    //decode thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&workder_thread, &attr, trampoline, this);
}

void *video_decoder::trampoline(void *p) {
    const char *url = ((video_decoder *) p)->url;
    gl_looper *looper = ((video_decoder *) p)->looper;
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
    //获取视频流的索引位置
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
    AVCodecContext *codecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codecContext, formatContext->streams[video_stream_index]->codecpar);
    AVCodec *videoCodec = avcodec_find_decoder(codecContext->codec_id);
    //打开解码器
    if (avcodec_open2(codecContext, videoCodec, nullptr) < 0) {
        ALOGD("can not open video decoder")
        return nullptr;
    }
    //循环从文件读取一帧压缩数据
    int size = codecContext->width * codecContext->height;
    auto *packet = static_cast<AVPacket *>(malloc(sizeof(AVPacket)));
    av_new_packet(packet, size);
//    AVFrame *yuvFrame = av_frame_alloc();
    //缓冲区分配内存
//    int numBytes=av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height, 1);
//    uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
//    av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, buffer, AV_PIX_FMT_YUV420P,
//                         codecContext->width, codecContext->height, 1);
//    //用于转码（缩放）的参数，转之前的宽高，转之后的宽高，格式等
//    struct SwsContext *sws_ctx = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
//                                                codecContext->width, codecContext->height, AV_PIX_FMT_YUV420P,
//                                                SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    int ret;
    while (codecContext->pix_fmt == AV_PIX_FMT_YUV420P) {
        if (av_read_frame(formatContext, packet) < 0) {
            ALOGD("read frame end")
            break;
        }
        //视频流
        if (packet->stream_index == video_stream_index) {
            //7.解码一帧视频压缩数据，得到视频像素数据
            AVFrame *pFrame = av_frame_alloc();
            ret = avcodec_send_packet(codecContext, packet);
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
//            sws_scale(sws_ctx, (const uint8_t *const *) yuvFrame->data, yuvFrame->linesize, 0, codecContext->height,
//                      pFrame->data, pFrame->linesize);
            looper->postMessage(looper->kMsgSurfaceDoFrame, pFrame);
            av_packet_unref(packet);
        }
    }
    av_packet_free(&packet);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
    return nullptr;
}
