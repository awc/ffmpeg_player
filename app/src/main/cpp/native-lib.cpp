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
#include "audio/audio_looper.h"
#include "offscreen/off_screen_thread.h"
#include "offscreen/on_screen_thread.h"

static JavaVM *g_vm;
bool destoryed = false;
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegplayer_MainActivity_stringFromJNI(JNIEnv *env, jobject instance) {
    return env->NewStringUTF(avcodec_configuration());
}

//surface

gl_looper *glLooper = nullptr;
audio_looper *audioLooper = nullptr;
ANativeWindow *nativeWindow = nullptr;
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeSurfaceCreated(JNIEnv *env, jobject instance, jobject surface) {
    destoryed = false;
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
    destoryed = true;
    if (nativeWindow != nullptr) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = nullptr;
    }
    if (glLooper != nullptr) {
        glLooper->postMessage(glLooper->kMsgSurfaceDestroyed);
        glLooper->quit();
        delete glLooper;
        glLooper = nullptr;
    }
    if (audioLooper != nullptr) {
        audioLooper->postMessage(audioLooper->kMsgAudioPlayerDestroyed);
        audioLooper->quit();
        delete audioLooper;
        audioLooper = nullptr;
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
    videoDecoder->vm = g_vm;
    audioDecoder = new audio_decoder();

    video_queue = new circle_av_frame_queue();
    audio_queue = new circle_av_frame_queue();

    synchronizer = new video_audio_synchronizer();
}

int64_t start_time = 0;
bool renderFirstFrame = false;
jobject javaPlayerRef = nullptr;
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativeSurfaceView_nativeDoFrame(JNIEnv *env, jobject instacne, jlong frameTimeMillis) {
    if (destoryed) {
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "nativeDoFrame", "");
    int64_t pts = video_queue->pullAVFramePts();
    if (pts == 0) {
        start_time = frameTimeMillis;
    }
    if (synchronizer->syncVideo(pts, frameTimeMillis - start_time)) {
        AVFrame *frame = video_queue->pull();
        if (frame != nullptr) {
            glLooper->postMessage(glLooper->kMsgSurfaceDoFrame, frame);

            if (!renderFirstFrame && javaPlayerRef != nullptr) {
                jclass clazz = env->GetObjectClass(javaPlayerRef);
                jmethodID methodId = env->GetMethodID(clazz, "onRenderFirstFrame", "()V");
                env->CallVoidMethod(javaPlayerRef, methodId);
                renderFirstFrame = true;
            }
        }
    }

    int64_t audio_pts = audio_queue->pullAVFramePts();
    if (synchronizer->syncAudio(audio_pts, frameTimeMillis - start_time)) {
        audioLooper->postMessage(audioLooper->kMsgAudioPlayerDoFrame);

        if (!renderFirstFrame && javaPlayerRef != nullptr) {
            jclass clazz = env->GetObjectClass(javaPlayerRef);
            jmethodID methodId = env->GetMethodID(clazz, "onRenderFirstFrame", "()V");
            env->CallVoidMethod(javaPlayerRef, methodId);
            renderFirstFrame = true;
        }
    }
//    __android_log_print(ANDROID_LOG_DEBUG, "doFrameFail", " %lld, %lld", pts, frameTimeMillis - start_time);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerSetDataSource(JNIEnv *env, jobject instance, jstring url,
                                                                     jobject javaPlayer) {
    javaPlayerRef = env->NewGlobalRef(javaPlayer);
    const char *path = env->GetStringUTFChars(url, nullptr);
    if (videoDecoder != nullptr) {
        videoDecoder->decode(path, video_queue, javaPlayerRef);
        audioDecoder->decode(path, audio_queue, audioLooper);
    }
    //todo
//    env->ReleaseStringUTFChars(url, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerStart(JNIEnv *env, jobject instance) {

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerPause(JNIEnv *env, jobject instance) {

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativePlayerRelease(JNIEnv *env, jobject instance) {
    if (videoDecoder != nullptr) {
        delete videoDecoder;
        videoDecoder = nullptr;
    }
    if (audioDecoder != nullptr) {
        delete audioDecoder;
        audioDecoder = nullptr;
    }
    delete video_queue;
    video_queue = nullptr;
    delete audio_queue;
    audio_queue = nullptr;
    delete synchronizer;
    synchronizer = nullptr;

    if (javaPlayerRef != nullptr) {
        env->DeleteGlobalRef(javaPlayerRef);
        javaPlayerRef = nullptr;
    }
}

//audio
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_NativePlayer_nativeAudioInit(JNIEnv *env, jobject instance, jint defaultSampleRate,
                                                           jint defaultFramesPerBurst) {
    audioLooper = new audio_looper(audio_queue);
    audioLooper->postMessage(audioLooper->kMsgAudioPlayerCreated, defaultSampleRate, defaultFramesPerBurst,
                             audio_queue);
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

//offScreen
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_offScreen_OffScreenUtil_nativeStartOffScreenTask(JNIEnv *env, jobject instance,
                                                                               jstring path, jstring destPath) {
    const char *src = env->GetStringUTFChars(path, nullptr);
    const char *dest = env->GetStringUTFChars(destPath, nullptr);
    __android_log_print(ANDROID_LOG_DEBUG, "video path ", ": %s, %s", src, dest);
    off_screen_thread *offScreenThread = new off_screen_thread(src, dest, g_vm);
    env->ReleaseStringUTFChars(path, src);
    env->ReleaseStringUTFChars(destPath, dest);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegplayer_offScreen_OffScreenUtil_nativeStartOnScreenTask(JNIEnv *env, jobject instance,
                                                                              jstring path, jstring destPath,
                                                                              jobject surface) {
    const char *src = env->GetStringUTFChars(path, nullptr);
    const char *dest = env->GetStringUTFChars(destPath, nullptr);
    __android_log_print(ANDROID_LOG_DEBUG, "video path ", ": %s, %s", src, dest);
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    on_screen_thread *offScreenThread = new on_screen_thread(src, dest, g_vm, nativeWindow);
    env->ReleaseStringUTFChars(path, src);
    env->ReleaseStringUTFChars(destPath, dest);
}