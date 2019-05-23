//
// Created by templechen on 2019-05-22.
//

#include "on_screen_thread.h"

#include <pthread.h>
#include "../common/native_log.h"
#include "../filter/base_filter.h"
#include "../encoder/video_encoder.h"
#include "../egl/window_surface.h"
#include <GLES3/gl3.h>

extern "C" {
#include <libavformat/avformat.h>
}

on_screen_thread::on_screen_thread(const char *sourcePath, const char *destPath, JavaVM *vm, ANativeWindow *nativeWindow) {
    this->path = sourcePath;
    this->vm = vm;
    this->destPath = destPath;
    this->nativeWindow = nativeWindow;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker_thread, &attr, trampoline, this);
}

on_screen_thread::~on_screen_thread() {

}

void *on_screen_thread::trampoline(void *p) {
    JavaVM *vm = ((on_screen_thread *) p)->vm;
    const char *path = ((on_screen_thread *) p)->path;
    const char *destPath = ((on_screen_thread *) p)->destPath;
    JNIEnv *jniEnv = nullptr;
    if (vm != nullptr) {
        vm->AttachCurrentThread(&jniEnv, nullptr);
    }

    //封装格式上下文
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, path, nullptr, nullptr) < 0) {
//        ALOGD("can not open ", path)
        return nullptr;
    }
    //获取信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
//        ALOGD("can not find stream info", path)
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
    //初始化egl
    auto *eglCore = new egl_core(nullptr, FLAG_TRY_GLES3);
    ANativeWindow *nativeWindow = ((on_screen_thread *) p)->nativeWindow;
    auto *onScreenSurface = new window_surface(nativeWindow, eglCore, false);
    onScreenSurface->makeCurrent();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    auto *filter = new base_filter();
    filter->init_program();
    glViewport(0, 0, codecContext->width, codecContext->height);
    onScreenSurface->swapBuffer();

    //循环从文件读取一帧压缩数据
    int size = codecContext->width * codecContext->height;
    auto *packet = static_cast<AVPacket *>(malloc(sizeof(AVPacket)));
    av_new_packet(packet, size);

    double ratio = av_q2d(formatContext->streams[video_stream_index]->time_base) * 1000;
    int ret;

    //encoder
    auto *encoder = new video_encoder(destPath, codecContext->width, codecContext->height, codecContext->framerate);

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
            if (pFrame->pts == AV_NOPTS_VALUE) {
                pFrame->pts = static_cast<int64_t>(pFrame->best_effort_timestamp * ratio);
            } else {
                pFrame->pts = static_cast<int64_t>(pFrame->pts * ratio);
            }
            __android_log_print(ANDROID_LOG_DEBUG, "video", " %lld, %d", pFrame->pts, packet->size);
            auto pts = pFrame->pts;
            //gl
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            filter->drawFrame(pFrame);
            onScreenSurface->swapBuffer();

            //encode one frame
            encoder->encode_frame(pts);

            av_packet_unref(packet);
        }
    }
    av_packet_free(&packet);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    //gl
    delete filter;
    onScreenSurface->release();
    delete onScreenSurface;
    eglCore->release();
    delete eglCore;

    delete encoder;

    if (vm != nullptr) {
        vm->DetachCurrentThread();
    }

    __android_log_print(ANDROID_LOG_DEBUG, "video", "offScreenThread over");

    return nullptr;
}

