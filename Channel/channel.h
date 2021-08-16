#ifndef _Channel_
#define _Channel_
#include "pthread.h"
#include "queue.h"

typedef struct channel {
    int size;
    int elemSize;
    void* unbInfo;
    queue* bInfo;
    int isSending;
    int isReceiving;
    pthread_mutex_t lock;
    pthread_mutex_t sLock;
    pthread_mutex_t rLock;
    pthread_cond_t sCond;
    pthread_cond_t rCond;
} channel;

void ChannelInit(channel* c, int size, int elemSize);

void ChannelSend(channel* c, void* src);

void ChannelReceive(channel* c, void* dst);

void ChannelDispose(channel* c);

#endif