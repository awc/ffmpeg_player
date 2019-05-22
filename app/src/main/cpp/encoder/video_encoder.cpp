//
// Created by templechen on 2019-05-16.
//

#include <GLES3/gl3.h>
#include "video_encoder.h"
#include "../common/native_log.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile) {
    int ret = 0;
    if (frame && frame->data != nullptr) {
        ret = avcodec_send_frame(enc_ctx, frame);
        __android_log_print(ANDROID_LOG_DEBUG, "video", "avcodec_send_frame %d", ret);
    }
    if (ret < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "video", "avcodec_send_frame fail");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//            __android_log_print(ANDROID_LOG_DEBUG, "video", "avcodec_receive_packet end %d", ret);
//            return;
//        }
        if (ret == AVERROR(EAGAIN)) {
//            ALOGD(" output is not available in the current state - user must try to send input");
            return;
        }
        else if (ret == AVERROR_EOF) {
//            ALOGD(" the encoder has been fully flushed, and there will be no more output packets");
            return;
        }
        else if (ret < 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "video", "avcodec_receive_packet fail");
            exit(1);
        }
        fwrite(pkt->data, 1, pkt->size, outfile);
        __android_log_print(ANDROID_LOG_DEBUG, "video", "fwrite");
        av_packet_unref(pkt);
    }
}

video_encoder::video_encoder(const char *dest, int width, int height) {
//    f = fopen(dest, "w+");
    f = fopen("/sdcard/ffmpeg_test.h264", "w+");
    if (f == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "video", "can not open file %s", dest);
        return;
    }

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (codec == nullptr) {
        ALOGD("can not find h264 encoder")
        return;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (codecContext == nullptr) {
        ALOGD("can not allocate encoder context")
        return;
    }
    codecContext->bit_rate = 4000000;
    codecContext->width = width;
    codecContext->height = height;
    codecContext->time_base.num =1;
    codecContext->time_base.den =30;
//    codecContext->framerate = (AVRational) {25, 1};
    codecContext->gop_size = 250;
    codecContext->max_b_frames = 5;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    if (codec->id == AV_CODEC_ID_H264) {
        av_opt_set(codecContext->priv_data, "preset", "slow", 0);
    }
    int ret = avcodec_open2(codecContext, codec, nullptr);
    if (ret < 0) {
        ALOGD("can not open encoder")
        return;
    }

    swsContext = sws_getContext(codecContext->width, codecContext->height, AV_PIX_FMT_RGB24, codecContext->width,
                                codecContext->height, AV_PIX_FMT_YUV420P, SWS_POINT,
                                nullptr, nullptr, nullptr);
    ALOGD("video encoder init success")
}

video_encoder::~video_encoder() {

}

void video_encoder::encode_frame(int64_t pts) {
    AVFrame *frame = av_frame_alloc();
    frame->format = codecContext->pix_fmt;
    frame->width = codecContext->width;
    frame->height = codecContext->height;
    int ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        ALOGD("can not allocate video frame data")
        return;
    }

    ret = av_frame_make_writable(frame);
    if (ret < 0) {
        ALOGD("AVFrame can not write")
        return;
    }

    GLubyte *pixels = static_cast<GLubyte *>(malloc(4 * codecContext->width * codecContext->height));
    glReadPixels(0, 0, codecContext->width, codecContext->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    if (pixels == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "video", "pixels is nullptr");
    }

    uint8_t *inData[1] = {pixels};
    int inlineSize[1] = {4 * codecContext->width};
    sws_scale(swsContext, inData, inlineSize, 0, codecContext->height, frame->data, frame->linesize);
    frame->pts = pts;
    AVPacket *packet = av_packet_alloc();
    encode(codecContext, frame, packet, f);
    av_frame_unref(frame);

}
