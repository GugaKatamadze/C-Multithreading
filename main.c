#include "threadpool.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "future.h"
#include "channel.h"

int ans1 = 0;
int ans2 = 0;
pthread_mutex_t lock;

typedef struct argsForSum {
    int k;
    int a;
    int b;
    int ans;
} argsForSum;

void* sum(void* args) {
    //usleep(30000);
    int a = ((argsForSum*)args)->a;
    int b = ((argsForSum*)args)->b;
    ((argsForSum*)args)->ans = a + b;
    int k = ((argsForSum*)args)->k;
    printf("sum of %d and %d is %d\n", a, b, ((argsForSum*)args)->ans);
    pthread_mutex_lock(&lock);
    ans2 += k;
    pthread_mutex_unlock(&lock);
}

typedef struct argsForMerge {
    int k;
    char* a;
    char* b;
    char* ans;
} argsForMerge;

void* merge(void* args) {
    //usleep(30000);
    char* a = ((argsForMerge*)args)->a;
    char* b = ((argsForMerge*)args)->b;
    ((argsForMerge*)args)->ans = malloc(strlen(a) + strlen(b) + 1);
    strcpy(((argsForMerge*)args)->ans, a);
    strcat(((argsForMerge*)args)->ans, b);
    int k = ((argsForMerge*)args)->k;
    printf("%s\n", ((argsForMerge*)args)->ans);
    pthread_mutex_lock(&lock);
    ans2 += k;
    pthread_mutex_unlock(&lock);
}

void runThreadPool() {

    pthread_mutex_init(&lock, NULL);
    argsForSum* sums = malloc(sizeof(argsForSum) * 5001);
    argsForMerge* merges = malloc(sizeof(argsForMerge) * 5001);

    ThreadPool tp;
    ThreadPoolInit(&tp, 100);

    for (int k = 1; k <= 10000; k++) {
        ans1 += k;
    }

    for (int k = 1; k <= 5000; k++) {   
        sums[k].k = k;
        sums[k].a = rand() % 10000;
        sums[k].b = rand() % 10000;
        ThreadPoolSchedule(&tp, sum, &sums[k]);
        merges[k].k = 10000 - k + 1;
        merges[k].a = strdup("abcdefghijklm");
        merges[k].b = strdup("nopqrstuvwxyz");
        ThreadPoolSchedule(&tp, merge, &merges[k]);
    }

    ThreadPoolShutdown(&tp);
    printf("%d\n%d\n", ans1, ans2);

    free(sums);
    for (int k = 1; k <= 5000; k++) {
        free(merges[k].a);
        free(merges[k].b);
        free(merges[k].ans);
    }
    free(merges);
    pthread_mutex_destroy(&lock);

}

channel c1;

typedef struct channelArgs {
    int a;
    int b;
    int res;
    int thread;
} channelArgs;

void* testChannelSend(void* args) {
    int a = ((channelArgs*)args)->a;
    int b = ((channelArgs*)args)->b;
    ((channelArgs*)args)->res = a + b;
    int k = ((channelArgs*)args)->thread;
    ChannelSend(&c1, &k);
}

void* testChannelReceive(void* args) {
    int k = ((channelArgs*)args)->thread;
    ChannelReceive(&c1, &k);
}

void testChannelInt() {

    ChannelInit(&c1, 10, sizeof(int));

    pthread_t* threads = malloc(101 * sizeof(pthread_t));
    channelArgs* args = malloc(101 * sizeof(channelArgs));

    for (int k = 1; k <= 100; k++) {
        args[k].a = 1;
        args[k].b = 9;
        args[k].thread = k;
        if (k % 2) pthread_create(&threads[k], NULL, testChannelSend, &args[k]);
        else pthread_create(&threads[k], NULL, testChannelReceive, &args[k]);
    }

    for (int k = 1; k <= 100; k++) {
        pthread_join(threads[k], NULL);
    }

    ChannelDispose(&c1);
    free(threads);
    free(args);

}

channel c2;

typedef struct channelPointerArgs {
    char* a;
} channelPointerArgs;

void* pointerTestChannelSend(void* args) {
    char** a = &((channelPointerArgs*)args)->a;
    ChannelSend(&c2, a);
}

void* pointerTestChannelReceive(void* args) {
    char* d;
    ChannelReceive(&c2, &d);
}

void testChannelPointer() {

    ChannelInit(&c2, 0, sizeof(char*));

    pthread_t* threads = malloc(101 * sizeof(pthread_t));
    channelPointerArgs* pArgs = malloc(101 * sizeof(channelPointerArgs));

    for (int k = 1; k <= 100; k++) {
        pArgs[k].a = strdup("aaaaa");
        if (k % 2) pthread_create(&threads[k], NULL, pointerTestChannelSend, &pArgs[k]);
        else pthread_create(&threads[k], NULL, pointerTestChannelReceive, &pArgs[k]);
    }

    for (int k = 1; k <= 100; k++) {
        pthread_join(threads[k], NULL);
    }

    for (int k = 1; k <= 100; k++) {
        free(pArgs[k].a);
    }

    ChannelDispose(&c2);
    free(threads);
    free(pArgs);
}

void runChannel() {
    testChannelInt();
    //testChannelPointer();
}

typedef struct futureArgs {
    int a;
    int b;
    int res;
} futureArgs;

void* testFuture(void* args) {
    int a = ((futureArgs*)args)->a; 
    int b = ((futureArgs*)args)->b;
    ((futureArgs*)args)->res = a + b;
    return &(((futureArgs*)args)->res);
}

void runFuture() {
    future f;
    futureArgs* arg = malloc(sizeof(futureArgs));
    arg->a = rand() % 10000;
    arg->b = rand() % 10000;
    FutureInit(&f, testFuture, arg);
    int res = *(int*)FutureGetResult(&f);
    printf("sum of %d and %d is %d\n", arg->a, arg->b, res);
    free(arg);
}

int main() {

    runThreadPool();
    //runChannel();
    //runFuture();

}