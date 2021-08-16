#include "queue.h"
#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

void QueueNew(queue* q, int elemSize, QueueFreeFunction freefn, int initialAllocation) {
    assert(elemSize > 0);
    assert(initialAllocation >= 0);
    if (!initialAllocation) initialAllocation = 4;
    q->logicalLength = 0;
    q->allocatedLength = initialAllocation;
    q->elemSize = elemSize;
    q->freeFn = freefn;
    q->info = malloc(elemSize * initialAllocation);
}

void QueueDispose(queue* q) {
    if (q->freeFn != NULL) {
        for (int k = 0; k < q->logicalLength; k++) {
            void* ptr = (char*)q->info + k * q->elemSize;
            q->freeFn(ptr);
        }
    }
    free(q->info);
}

int QueueLength(queue* q) {
    return q->logicalLength;
}

void* QueueFront(queue* q) {
    assert(q->logicalLength > 0);
    return q->info;
}
	   
void QueuePop(queue* q) {
    assert(q->logicalLength > 0);
    if (q->freeFn != NULL) q->freeFn(q->info);
    memmove(q->info, (char*)q->info + q->elemSize, q->elemSize * (q->logicalLength - 1));
    q->logicalLength--;
}

void QueuePush(queue* q, void* elemAddr) {
    if (q->logicalLength == q->allocatedLength) {
        q->allocatedLength *= 2;
        q->info = realloc(q->info, q->elemSize * q->allocatedLength);
    }
    void* ptr = (char*)q->info + q->elemSize * q->logicalLength;
    memcpy(ptr, elemAddr, q->elemSize);
    q->logicalLength++;
}