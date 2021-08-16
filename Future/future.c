#include "future.h"

void* execute(void* args){
    ((future*)args)->res = ((future*)args)->fun(((future*)args)->args);
}

void FutureInit(future* f, funPtr fun, void* args) {
    f->fun = fun;
    f->args = args;
    pthread_create(&f->tid, NULL, execute, f);
}

void* FutureGetResult(future* f) {
    pthread_join(f->tid, NULL);
    return f->res;
}