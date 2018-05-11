#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "requests.h"

typedef struct {
	int maxSize;
	Request **intArray;
	int front;
	int rear;
	int itemCount;
}queue;


queue* qcreate(int max);
Request* qpeek(queue* q);
bool qisEmpty(queue* q);
bool qisFull(queue* q);
int qsize(queue* q);
void qinsert(queue* q, Request* data);
Request* qremoveData(queue* q);

#endif