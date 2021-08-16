#include "channel.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

void ChannelInit(channel* c, int size, int elemSize) {
    c->size = size;
    c->elemSize = elemSize;
    if (!size) c->unbInfo = malloc(elemSize);
    else {
        c->bInfo = malloc(sizeof(queue));
        QueueNew(c->bInfo, elemSize, NULL, size);
    }
    pthread_mutex_init(&c->lock, NULL);
    pthread_mutex_init(&c->sLock, NULL);
    pthread_mutex_init(&c->rLock, NULL);
    pthread_cond_init(&c->sCond, NULL);
    pthread_cond_init(&c->rCond, NULL);
}

void ChannelSend(channel* c, void* src) {
    if (!c->size) {
        pthread_mutex_lock(&c->sLock);
        pthread_mutex_lock(&c->lock);
        memcpy(c->unbInfo, src, c->elemSize);
        printf("sending %d\n", *(int*)src); // INT
        //printf("sending %s\n", *(char**)src); // CHAR*
        if (c->isReceiving) {
            pthread_cond_signal(&c->rCond);
            pthread_cond_wait(&c->sCond,&c->lock);
        }
        else {
            c->isSending = 1;
            pthread_cond_wait(&c->sCond, &c->lock);
        }
        pthread_mutex_unlock(&c->lock);
        pthread_mutex_unlock(&c->sLock);
    }
    else {
        pthread_mutex_lock(&c->sLock);
        pthread_mutex_lock(&c->lock);
        printf("want to send %d\n", *(int*)src); // INT
        if (QueueLength(c->bInfo) == c->size) {
            printf("waiting to send %d\n", *(int*)src); // INT
            pthread_cond_wait(&c->sCond, &c->lock);
        }
        printf("sending %d\n", *(int*)src); // INT
        //printf("sending %s\n", *(char**)src); // CHAR*
        QueuePush(c->bInfo, src);
        pthread_cond_signal(&c->rCond);
        pthread_mutex_unlock(&c->lock);
        pthread_mutex_unlock(&c->sLock);
    }
}

void ChannelReceive(channel* c, void* dst) {
    if (!c->size) {
        pthread_mutex_lock(&c->rLock);
        pthread_mutex_lock(&c->lock);
        if (c->isSending) {
            memcpy(dst, c->unbInfo, c->elemSize);
            c->isSending = 0;
            pthread_cond_signal(&c->sCond);
        }
        else {
            c->isReceiving = 1;
            pthread_cond_wait(&c->rCond, &c->lock);
            memcpy(dst, c->unbInfo, c->elemSize);
            c->isReceiving = 0;
            pthread_cond_signal(&c->sCond);
        }
        printf("received %d\n", *(int*)c->unbInfo); // INT
        //printf("received %s\n", *(char**)c->unbInfo); // CHAR*
        pthread_mutex_unlock(&c->lock);
        pthread_mutex_unlock(&c->rLock);
    }
    else {
        pthread_mutex_lock(&c->rLock);
        pthread_mutex_lock(&c->lock);
        printf("%d wants to receive\n", *(int*)dst); // INT
        if (!QueueLength(c->bInfo)) {
            printf("%d is waiting to receive\n", *(int*)dst); // INT
            pthread_cond_wait(&c->rCond, &c->lock);
            printf("%d woke up\n", *(int*)dst); // INT
        }
        printf("%d received %d\n", *(int*)dst, *(int*)QueueFront(c->bInfo)); // INT
        //printf("received %s\n", *(char**)QueueFront(c->bInfo)); // CHAR*
        memcpy(dst, QueueFront(c->bInfo), c->elemSize);
        QueuePop(c->bInfo);
        pthread_cond_signal(&c->sCond);
        pthread_mutex_unlock(&c->lock);
        pthread_mutex_unlock(&c->rLock);
    }
}

void ChannelDispose(channel* c) {
    if (!c->size) free(c->unbInfo);
    else {
        QueueDispose(c->bInfo);
        free(c->bInfo);
    }
    pthread_mutex_destroy(&c->lock);
    pthread_mutex_destroy(&c->sLock);
    pthread_mutex_destroy(&c->rLock);
    pthread_cond_destroy(&c->sCond);
    pthread_cond_destroy(&c->rCond);
}