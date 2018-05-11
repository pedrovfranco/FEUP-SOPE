#include "queue.h"

queue* qcreate(int max)
{
    queue* ret = malloc(sizeof(queue));

    ret->maxSize = max;
    ret->intArray = malloc(sizeof(Request*) * ret->maxSize);
    ret->front = 0;
    ret->rear = -1;
    ret->itemCount = 0;

    return ret;
}


Request* qpeek(queue* q) {
   return q->intArray[q->front];
}

bool qisEmpty(queue* q) {
   return q->itemCount == 0;
}

bool qisFull(queue* q) {
   return q->itemCount == q->maxSize;
}

int qsize(queue* q) {
   return q->itemCount;
}  

void qinsert(queue* q, Request* data) {

   if(!qisFull(q)) {
    
      if(q->rear == q->maxSize-1) {
         q->rear = -1;            
      }       

      q->intArray[++q->rear] = data;
      q->itemCount++;
   }
}

Request* qremoveData(queue* q) {
   Request* data = q->intArray[q->front++];
    
   if(q->front == q->maxSize) {
      q->front = 0;
   }
    
   q->itemCount--;
   return data;  
}