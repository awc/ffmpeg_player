//
// Created by templechen on 2019-06-03.
//

#include <pthread.h>
#include "circle_av_packet_queue.h"

circle_av_packet_queue::circle_av_packet_queue() {
    pthread_mutex_init(&mLock, nullptr);
    pthread_cond_init(&mCondition, nullptr);

    tail = new H264Packet();
    H264Packet *nextCursor = tail;
    H264Packet *curCursor = nullptr;
    int i = DEFAULT_PACKET_SIZE - 1;
    while (i > 0) {
        curCursor = new H264Packet();
        curCursor->next = nextCursor;
        nextCursor = curCursor;
        i--;
    }
    head = curCursor;
    tail->next = head;
    pushCursor = head;
    pullCursor = head;

    currSize = 0;
}

circle_av_packet_queue::~circle_av_packet_queue() {
    pthread_mutex_lock(&mLock);
    H264Packet *temp = head;
    while (head != tail) {
        if (head->packet != nullptr) {
            av_packet_unref(head->packet);
            temp = head;
            head = head->next;
            delete temp;
        }
    }
    if (tail->next != nullptr) {
        av_packet_unref(tail->packet);
    }
    delete tail;
    head = nullptr;
    tail = nullptr;
    pullCursor = nullptr;
    pushCursor = nullptr;
}

void circle_av_packet_queue::push(AVPacket *packet) {
    pthread_mutex_lock(&mLock);
    if (pushCursor->next == pullCursor) {
        pthread_cond_wait(&mCondition, &mLock);
    }
    pushCursor->packet = packet;
    pushCursor = pushCursor->next;
    currSize++;
    pthread_mutex_unlock(&mLock);
}

AVPacket *circle_av_packet_queue::pull() {
    if (currSize < 10 && pullCursor->next != pushCursor) {
        AVPacket *packet = pullCursor->packet;
        pullCursor = pullCursor->next;
        currSize--;
        return packet;
    } else {
        pthread_cond_signal(&mCondition);
        return nullptr;
    }
}
