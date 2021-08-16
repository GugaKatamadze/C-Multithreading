#include "threadpool.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

void* workThread(void* args) {
    ThreadPool* tp = (ThreadPool*)args;
    Task task;
    while (1) {
        pthread_mutex_lock(&tp->lock);
        while (!QueueLength(tp->tasks)) {
            if (tp->stop) {
                pthread_mutex_unlock(&tp->lock);
                return NULL;
            }
            pthread_cond_wait(&tp->cond, &tp->lock);
        }
        task = *(Task*)QueueFront(tp->tasks);
        QueuePop(tp->tasks);
        pthread_mutex_unlock(&tp->lock);
        task.taskFn(task.args);
    }
}

void ThreadPoolInit(ThreadPool* tp, int poolSize) {
    assert(poolSize > 0);
    tp->poolSize = poolSize;
    tp->threads = malloc(sizeof(pthread_t) * poolSize);
    tp->tasks = malloc(sizeof(queue));
    QueueNew(tp->tasks, sizeof(Task), NULL, 0);
    pthread_mutex_init(&tp->lock, NULL);
    pthread_cond_init(&tp->cond, NULL);
    tp->stop = 0;
    for (int k = 0; k < poolSize; k++) {
        pthread_create(&tp->threads[k], NULL, &workThread, tp);
    }
}

void ThreadPoolSchedule(ThreadPool* tp, taskFunction taskFn , void* args) {
    Task newTask;
    newTask.taskFn = taskFn;
    newTask.args = args;
    pthread_mutex_lock(&tp->lock);
    QueuePush(tp->tasks, &newTask);
    pthread_cond_signal(&tp->cond);
    pthread_mutex_unlock(&tp->lock);
}

void ThreadPoolShutdown(ThreadPool* tp) {
    pthread_mutex_lock(&tp->lock);
    tp->stop = 1;
    pthread_mutex_unlock(&tp->lock);
    pthread_cond_broadcast(&tp->cond);
    for (int k = 0; k < tp->poolSize; k++) {
        pthread_join(tp->threads[k], NULL);
    }
    pthread_mutex_destroy(&tp->lock);
    pthread_cond_destroy(&tp->cond);
    free(tp->threads);
    QueueDispose(tp->tasks);
    free(tp->tasks);
}