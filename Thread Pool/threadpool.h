#ifndef _ThreadPool_
#define _ThreadPool_
#include "queue.h"
#include "pthread.h"

typedef void* (*taskFunction)(void* args);

typedef struct ThreadPool{
    int poolSize;
    pthread_t* threads;
    queue* tasks;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int stop;
} ThreadPool;

typedef struct Task {
    taskFunction taskFn;
    void* args;
} Task;

void ThreadPoolInit(ThreadPool* tp, int poolSize);

void ThreadPoolSchedule(ThreadPool* tp, taskFunction taskFn , void* args);

void ThreadPoolShutdown(ThreadPool* tp);

#endif