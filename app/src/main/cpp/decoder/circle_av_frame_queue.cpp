//
// Created by templechen on 2019-05-06.
//

#include <pthread.h>
#include <android/log.h>
#include "circle_av_frame_queue.h"

extern "C" {
#include "libavformat/avformat.h"
}

circle_av_frame_queue::circle_av_frame_queue() {
    pthread_mutex_init(&mLock, nullptr);
    pthread_cond_init(&mCondition, nullptr);

    tail = new H264Frame();
    H264Frame *nextCursor = tail;
    H264Frame *curCursor = nullptr;
    int i = CIRCLE_QUEUE_DEFAULT_SIZE - 1;
    while (i > 0) {
        curCursor = new H264Frame();
        curCursor->next = nextCursor;
        nextCursor = curCursor;
        i--;
    }
    head = curCursor;
    tail->next = head;
    pushCursor = head;
    pullCursor = head;

    isAvailable = true;
}

circle_av_frame_queue::~circle_av_frame_queue() {
    __android_log_print(ANDROID_LOG_DEBUG, "video", "pthread_mutex_delete");
    pthread_mutex_lock(&mLock);
    H264Frame *node = head;
    H264Frame *temp;
    while (node != tail) {
        temp = node->next;
        if (node->frame != nullptr) {
            av_frame_free(&node->frame);
            node->frame = nullptr;
        }
        node->next = nullptr;
        delete node;
        node = temp;
    }
    if (tail->frame != nullptr) {
        av_frame_free(&tail->frame);
        tail->frame = nullptr;
        delete tail;
    }
    head = nullptr;
    tail = nullptr;
    pushCursor = nullptr;
    pullCursor = nullptr;
    isAvailable = false;
    pthread_mutex_unlock(&mLock);

    __android_log_print(ANDROID_LOG_DEBUG, "video", "pthread_mutex_destroy");
    pthread_cond_destroy(&mCondition);
    pthread_mutex_destroy(&mLock);
}

void circle_av_frame_queue::push(AVFrame *avFrame) {
    if (!isAvailable) {
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "video", "pthread_mutex_lock");
    pthread_mutex_lock(&mLock);
    if (pushCursor->next == pullCursor) {
        pthread_cond_wait(&mCondition, &mLock);
    }
    pushCursor->frame = avFrame;
    pushCursor = pushCursor->next;
    pthread_mutex_unlock(&mLock);
}

AVFrame *circle_av_frame_queue::pull() {
    if (!isAvailable) {
        return nullptr;
    }
    if (pullCursor->next != pushCursor) {
        AVFrame *frame = pullCursor->frame;
        pullCursor = pullCursor->next;
        return frame;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "video", "pthread_cond_signal");
        pthread_cond_signal(&mCondition);
        return nullptr;
    }
}

int64_t circle_av_frame_queue::pullAVFramePts() {
    if (!isAvailable) {
        return -1;
    }
    if (pullCursor->frame != nullptr) {
        return pullCursor->frame->pts;
    }
    return -1;
}


