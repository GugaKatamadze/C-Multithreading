#ifndef _Future_
#define _Future_
#include "pthread.h"

typedef void* (*funPtr)(void* args);

typedef struct future {
    funPtr fun;
    void* args;
    pthread_t tid;
    void* res;
} future;

void FutureInit(future* f, funPtr fun, void* args);

void* FutureGetResult(future* f);

#endif