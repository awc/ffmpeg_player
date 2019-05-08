//
// Created by templechen on 2019-04-28.
//

#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>

extern "C" {
#include "libavcodec/avcodec.h"
}

#include "render/gl_looper.h"
#include "decoder/video_decoder.h"
#include "decoder/audio_decoder.h"
#include "decoder/circle_av_frame_queue.h"
#include "synchronize/video_audiio_synchronizer.h"

static JavaVM *g_vm;
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegplayer_MainActivity_stringFromJNI(JNIEnv *env, jobject instance) {
    return env->NewStringUTF(avcodec_configuration());
}

//surface

gl_looper *glLooper = nullptr;
ANativeWindow *nativeWindow = nullptr;
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeSurfaceCreated(JNIEnv *env, jobject instance, jobject surface) {
    glLooper = new gl_looper();
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    glLooper->postMessage(glLooper->kMsgSurfaceCreated, nativeWindow);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeSurfaceChanged(JNIEnv *env, jobject instance, jint width,
                                                                     jint height) {
    if (glLooper != nullptr) {
        glLooper->postMessage(glLooper->kMsgSurfaceChanged, width, height);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeDestroyed(JNIEnv *env, jobject instance) {
    if (glLooper != nullptr) {
        glLooper->postMessage(glLooper->kMsgSurfaceDestroyed);
        glLooper->quit();
        delete glLooper;
        glLooper = nullptr;
    }
    if (nativeWindow != nullptr) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = nullptr;
    }
}

//player

video_decoder *videoDecoder = nullptr;
audio_decoder *audioDecoder = nullptr;
circle_av_frame_queue *video_queue = nullptr;
circle_av_frame_queue *audio_queue = nullptr;
video_audio_synchronizer *synchronizer = nullptr;
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerInit(JNIEnv *env, jobject instance) {
    videoDecoder = new video_decoder();
    audioDecoder = new audio_decoder();

    video_queue = new circle_av_frame_queue();
    audio_queue = new circle_av_frame_queue();

    synchronizer = new video_audio_synchronizer();
}

int64_t start_time = 0;
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeDoFrame(JNIEnv *env, jobject instacne, jlong frameTimeMillis) {
    int64_t pts = video_queue->pullAVFramePts();
    if (pts == 0) {
        start_time = frameTimeMillis;
    }
    if (synchronizer->syncVideo(pts, frameTimeMillis - start_time)) {
        AVFrame *frame = video_queue->pull();
        if (frame != nullptr) {
            __android_log_print(ANDROID_LOG_DEBUG, "doFrameSuccess", " %lld, %lld", pts, frameTimeMillis - start_time);
            glLooper->postMessage(glLooper->kMsgSurfaceDoFrame, frame);
        }
    }
    __android_log_print(ANDROID_LOG_DEBUG, "doFrameFail", " %lld, %lld", pts, frameTimeMillis - start_time);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerSetDataSource(JNIEnv *env, jobject instance, jstring url) {
    const char *path = env->GetStringUTFChars(url, nullptr);
    if (videoDecoder != nullptr) {
        videoDecoder->decode(path, glLooper, video_queue);
        audioDecoder->decode(path);
    }
    env->ReleaseStringUTFChars(url, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerStart(JNIEnv *env, jobject instance) {

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerPause(JNIEnv *env, jobject instance) {

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerRelease(JNIEnv *env, jobject instance) {
    delete videoDecoder;
    videoDecoder = nullptr;
    delete audioDecoder;
    audioDecoder = nullptr;
    delete video_queue;
    video_queue = nullptr;
    delete audio_queue;
    audio_queue = nullptr;
    delete synchronizer;
    synchronizer = nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(
        JavaVM *vm, void *reserved
) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}


void JNI_OnUnload(JavaVM *vm, void *reserved) {
    g_vm = nullptr;
}
