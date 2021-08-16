#ifndef _queue_
#define _queue_

typedef void (*QueueFreeFunction)(void* elemAddr);

typedef struct {
  int logicalLength;
  int allocatedLength;
  int elemSize;
  void* info;
  QueueFreeFunction freeFn;
} queue;

void QueueNew(queue* q, int elemSize, QueueFreeFunction freefn, int initialAllocation);

void QueueDispose(queue* q);

int QueueLength(queue* q);

void* QueueFront(queue* q);
	   
void QueuePop(queue* q);
					  
void QueuePush(queue* q, void* elemAddr);

#endif